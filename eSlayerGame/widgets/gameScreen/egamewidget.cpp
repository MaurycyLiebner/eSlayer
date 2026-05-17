#include "egamewidget.h"

#include "../../emainwindow.h"

#include "../../textures/eobjstextures.h"
#include "../../textures/eterrstextures.h"
#include "../../textures/emissilestextures.h"
#include "../../textures/etextgenerator.h"
#include "../../textures/eitemstextures.h"

#include "../../names/eareanames.h"
#include "../../names/eobjectnames.h"

#include "../../erendersettings.h"
#include "../../elanguage.h"

#include "eunitindicator.h"
#include "ehoverwidget.h"
#include "einventorywidget.h"
#include "eminimap.h"

#include <eSlayerMissiles/emissileincrement.h>

#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/earea.h>
#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/estringhelpers.h>
#include <eSlayerHelpers/eobjectsinfo.h>

eGameWidget* eGameWidget::sInstance = nullptr;

eGameWidget::eGameWidget(eMainWindow* const window) :
    eLabel(window),
    mWorld(mMap),
    mInput(resolution().tileWidth(),
           resolution().tileHeight()),
    mGamePainter(mTileIterator, renderer()) {
    sInstance = this;
    setNoPadding();
}

eGameWidget::~eGameWidget() {
    sInstance = nullptr;
}

void eGameWidget::initialize(const int clientId,
                             const std::shared_ptr<eServer>& server,
                             const std::shared_ptr<eMap>& map,
                             const eCharacter& c,
                             const eTeamId teamId) {
    mCName = c.name();
    mUserNames[clientId] = mCName;
    mHardcore = c.hardcore();

    mClientId = clientId;
    mServer = server;
    mMap = map;

    initializeTextures();

    const auto r = renderer();
    const auto& res = resolution();

    const auto w = [this](const ePointF& pos) {
        return mMap->walkable(pos);
    };
    const auto iter = [this](const ePointF& pos,
                             const float dist,
                             const eOtherHandler& handler) {
        for(const auto& u : mWorld.units()) {
            if(!u) continue;
            handler(*u);
        }
    };
    mMainAction.initialize(mServer, res, r, w, iter, clientId, teamId);
    mMainChar = mMainAction.unit();
    mMainChar->fPos = map->spawnPos();

    setRightSkill(0);
    setLeftSkill(0);

    const auto& srcEq = c.equipment();
    auto& dstEq = mMainAction.equipment();
    dstEq = srcEq;
    const auto& srcAttrs = c.attributes();
    auto& dstAttrs = mMainAction.attributes();
    dstAttrs = srcAttrs;
    auto& dstStats = mMainAction.stats();
    dstStats.fBaseSkillLevels = c.skillLevels();
    dstStats.calculate(dstAttrs, dstEq);

    mWorld.initialize(clientId, mMainChar);

    const int fontSize = res.smallFontSize();
    const auto font = eFonts::textFont(fontSize);
    mItemNames.initialize(r, font);

    mTileIterator.initialize(this);
}

const ePointF& eGameWidget::characterPos() const {
    return mMainAction.pos();
}

ePointF eGameWidget::pixelToTilePos(
    const ePointF& pos,
    const ePointF& pixel) const {
    return mInput.pixelToTilePos(pos, pixel, width(), height());
}

ePointF eGameWidget::pixelToTilePos(
    const ePointF& pixel) const {
    const auto& pos = characterPos();
    return mInput.pixelToTilePos(pos, pixel, width(), height());
}

ePointF eGameWidget::tilePosToPixel(const ePointF& pos) const {
    const auto& charPos = characterPos();
    return mInput.tilePosToPixel(pos, charPos, width(), height());
}

void eGameWidget::setUnitIndicator(eUnitIndicator* const indicator) {
    mUnitIndicator = indicator;
}

void eGameWidget::stop() {
    mMainAction.stop();
    mMainAction.stand();
}

std::string eGameWidget::userName(const int clientId) const {
    const auto it = mUserNames.find(clientId);
    if(it == mUserNames.end()) return "";
    return it->second;
}

void eGameWidget::dropItem() {
    auto& eq = mMainAction.equipment();
    auto& dragged = eq.fDragged;
    if(dragged.fType == eItemType::none) return;
    mServer->dropItem(mClientId);
    dragged = eItem();
    eHoverWidget::sUpdateDragItem(eq);
}

void eGameWidget::sendInventoryRearranged() {
    const auto& eq = mMainAction.equipment();
    mServer->rearrangeItems(mClientId, eq);
}

void eGameWidget::sendAttributesChanged() {
    const auto& attrs = mMainAction.attributes();
    mServer->changeAttributes(mClientId, attrs);
}

void eGameWidget::sendSkillLevelsChanged() {
    const auto& stats = mMainAction.stats();
    const auto& skillLevels = stats.fBaseSkillLevels;
    mServer->changeSkillLevels(mClientId, skillLevels);
}

void eGameWidget::setLeftSkill(const int s) {
    if(mLeftSkill == s) return;
    mLeftSkill = s;
    mMainAction.setSkillId(eSkillChoice::left, s);
    mServer->setSkillId(mClientId, eSkillChoice::left, s);
}

void eGameWidget::setRightSkill(const int s) {
    if(mRightSkill == s) return;
    mRightSkill = s;
    mMainAction.setSkillId(eSkillChoice::right, s);
    mServer->setSkillId(mClientId, eSkillChoice::right, s);
}

void eGameWidget::respawn() {
    mServer->respawn(mClientId);
    auto& eq = equipment();
    eq = eEquipment();
    mMainAction.recalculateStats();
}

bool eGameWidget::switchRunning() {
    const bool run = !mMainAction.running();
    mMainAction.setRunning(run);
    return run;
}

bool eGameWidget::switchWeapons() {
    auto& eq = mMainAction.equipment();
    eq.fWeapons1 = !eq.fWeapons1;
    sSendInventoryRearranged();
    return eq.fWeapons1;
}

void eGameWidget::disconnect() {
    if(mServer) {
        mServer->disconnect(mClientId);
    }
}

void eGameWidget::save() {
    const eCharacter c(mCName, mHardcore);
    const auto path = eGameDir::path(
        "Save/" + mCName + ".xml");
    const auto& eq = equipment();
    const auto& attrs = attributes();
    const auto& stats = eGameWidget::stats();
    const auto& skillLevels = stats.fBaseSkillLevels;
    c.write(path, eq, attrs, skillLevels);
}

void eGameWidget::sendMessage(const std::string& text) {
    mServer->sendMessage(mClientId, text);
}

void eGameWidget::consumePotion(const int x) {
    auto& eq = equipment();
    const auto p = eq.takePotion(x);
    return consumePotion(p);
}

void eGameWidget::consumePotion(const eItem& p) {
    if(p.fType != eItemType::potion) return;
    mServer->consumePotion(mClientId, p.fItemId);
}

void eGameWidget::sSendInventoryRearranged() {
    sInstance->sendInventoryRearranged();
    sInstance->mMainAction.recalculateStats();
}

void eGameWidget::sSendSkillLevelsChanged() {
    sInstance->sendSkillLevelsChanged();
    sInstance->mMainAction.recalculateStats();
}

void eGameWidget::sSendAttributesChanged() {
    sInstance->sendAttributesChanged();
    sInstance->mMainAction.recalculateStats();
}

void eGameWidget::paintEvent(ePainter& p) {
    mGamePainter.clear();

    const float by = 1.f;
    mServer->increment(by);

    const auto r = renderer();

    {
        const auto newUsers = mServer->receiveNewUsers();
        for(const auto& u : newUsers) {
            const auto& name = u.fName;
            mUserNames[u.fClientId] = name;
            if(u.fJustJoined) {
                auto text = eLanguage::text(12, 0);
                text = eStringHelpers::replaceAll(text, "%1", name);
                addMessage(r, text);
            }
        }
        const auto leftUsers = mServer->receiveLeftUsers();
        for(const int clientId : leftUsers) {
            const auto& name = mUserNames[clientId];
            auto text = eLanguage::text(12, 1);
            text = eStringHelpers::replaceAll(text, "%1", name);
            addMessage(r, text);
        }
        const auto messages = mServer->receiveMessages();
        for(const auto& msg : messages) {
            const int clientId = msg.fClientId;
            const auto& name = mUserNames[clientId];
            const auto text = name + ": " + msg.fMsg;
            addMessage(r, text);
        }
        const auto objs = mServer->receiveObjectStateChanges();
        for(const auto& obj : objs) {
            const auto o = mMap->object(obj.fPos, obj.fObjectId);
            if(!o) continue;
            o->fState = obj.fState;
        }
        const auto doors = mServer->receiveDoorsStateChanges();
        for(const auto& d : doors) {
            for(const auto& t : d.fTiles) {
                auto& tile = mMap->tile(t.fX, t.fY);
                switch(d.fType) {
                case eWallType::topLeft: {
                    eTile::setOpen(tile.fWallTL, d.fOpen);
                } break;
                case eWallType::topRight: {
                    eTile::setOpen(tile.fWallTR, d.fOpen);
                } break;
                }
            }
        }
    }
    const auto& res = resolution();
    const auto worldResult = mWorld.processServerData(
        mClientId, *mServer, *mMainChar, mMainAction, res, r);
    if(eInventoryWidget::sBlocked) {
        auto& eq = mMainAction.equipment();
        const bool r = mServer->receiveEquipment(mClientId, eq);
        if(r) {
            mMainAction.recalculateStats();
            eInventoryWidget::sBlocked = false;
            eHoverWidget::sUpdateDragItem(eq);
        } else {
            const bool r = mServer->unblockEquipment(mClientId);
            if(r) {
                eInventoryWidget::sBlocked = false;
            }
        }
    }

    if(worldResult.fReceived) {
        if(worldResult.fHasMainCharData) {
            const auto& u = worldResult.fMainCharData;
            if(mMainChar->fHealth <= 0 && u.fHealth > 0) {
                mMainAction.setPos(u.fPos);
                mMainAction.stop();
                setPressedUnit(nullptr);
                setHighlightedUnit(nullptr);
                setHighlightedObject(nullptr);
                if(mRespawnHandler) mRespawnHandler();
            }
            if(u.fBlockingActionTime > 0) {
                mMainChar->fPos = u.fPos;
            }
            mMainChar->fHealth = u.fHealth;
            {
                auto& stats = eGameWidget::stats();
                auto& attrs = eGameWidget::attributes();
                stats.fHealthF = u.fHealth;
                stats.fManaF = worldResult.fMana;
                attrs.fExp = worldResult.fExperience;
                const auto oldLevel = attrs.fLevel;
                const auto newLevel = worldResult.fLevel;
                if(oldLevel != newLevel) {
                    attrs.fLevel = newLevel;
                    const auto& eq = eGameWidget::equipment();
                    stats.calculate(attrs, eq);
                }
            }
            if(u.fHealth <= 0) {
                if(mDeathHandler) mDeathHandler();
                mMainAction.stop();
            }
            mMainChar->fMaxHealth = u.fMaxHealth;
            mMainChar->fBlockingActionTime = u.fBlockingActionTime;
            if(u.fAnimId > mMainChar->fAnimId) {
                mMainChar->fAnim = u.fAnim;
                mMainChar->fAnimId = u.fAnimId;
                mMainChar->fAnimSpeed = u.fAnimSpeed;
            }
        }

        auto& model = mMainChar->model();
        model.setAggressive(worldResult.fAggressive);
    }

    const auto upos = mMainChar->fPos;
    const auto uipos = upos.floor();
    const int areaId = mMap->areaAt(uipos);
    if(mLastArea != areaId && areaId >= 0) {
        mLastArea = areaId;
        const auto areaNameBase = mMap->areaName(areaId);
        const auto area = mMap->area(areaId);
        const auto areaName = eAreaNames::name(areaNameBase);
        eMiniMap::sShowAreaName(areaName);
        mGamePainter.setLightness(area.fLightness);
        mGamePainter.setContrast(area.fContrast);
    }

    mServer->changeState(mClientId, *mMainChar);

    mWorld.simulateMissiles(by);
    mWorld.simulateNovas(by);

    const auto& mpos = mInput.mousePos();
    if(!mMenuVisible) {
        const auto mouseTilePos = pixelToTilePos(mpos);
        const auto w = window();
        const bool shiftPressed = w->shiftPressed();
        mMainAction.increment(mInput.mousePressed(),
                              mInput.rightPressed(),
                              shiftPressed,
                              mouseTilePos,
                              by);
    }

    mFrame++;

    const int tileW = eGameWidget::tileWidth();
    const int tileH = eGameWidget::tileHeight();
    {
        const auto holder = mGamePainter.switchToBase();

        const auto& terrTypes = mMap->terrainTypes();
        const auto& objTypes = mMap->objectTypes();


        enum class eRenderElementType {
            unit, missile, item, object, wall
        };

        struct eRenderElement {
            eRenderElementType fType;
            std::shared_ptr<ePositioned> fPtr;
            std::shared_ptr<eTexture> fTex;
        };

        struct eWall : public ePositioned {
            int fTerrainType;
            eWallType fType;
            uint8_t fEncoded;
        };

        std::vector<eRenderElement> renderElements;

        int wallMinTX = uipos.fX - 1000;
        int wallMaxTX = uipos.fX + 1000;
        int wallMinTY = uipos.fY - 1000;
        int wallMaxTY = uipos.fY + 1000;
        const auto handleTile = [&](const eTileInfo& info) {
            const int x = info.fTX;
            const int y = info.fTY;
            const auto& iobjs = mMap->objects(x, y);
            for(const auto& iobj : iobjs) {
                const auto& obj = mMap->object(iobj);
                const auto& objRef = *obj;
                const auto objType = objRef.fObjectType;
                const auto& object = eObjectsInfo::sObjects.get(objType);
                const auto texObjectId = object.fTexId;
                const auto& objectTex = eObjsTextures::get(texObjectId);
                if(objectTex.fBlocksLight) {
                    const auto& pos = objRef.fPos;
                    mGamePainter.addObjectShadow(pos.fX, pos.fY, object.fSize);
                }
                renderElements.emplace_back(eRenderElement{eRenderElementType::object,
                                                           std::static_pointer_cast<ePositioned>(obj)});
            }
            const auto& tile = mMap->tile(x, y);
            const auto addWall = [&](const eWallType wallType,
                                     const uint8_t encoded) {
                const auto wall = std::make_shared<eWall>();
                wall->fPos = ePointF{x, y};
                wall->fTerrainType = tile.fTerrainType;
                wall->fType = wallType;
                wall->fEncoded = encoded;
                switch(wallType) {
                case eWallType::topRight: {
                    if(x == uipos.fX) {
                        if(y > uipos.fY) {
                            wallMaxTY = std::min(wallMaxTY, y);
                        } else {
                            wallMinTY = std::max(wallMinTY, y);
                        }
                    }
                } break;
                case eWallType::topLeft: {
                    if(y == uipos.fY) {
                        if(x > uipos.fX) {
                            wallMaxTX = std::min(wallMaxTX, x);
                        } else {
                            wallMinTX = std::max(wallMinTX, x);
                        }
                    }
                } break;
                };

                const auto terrType = tile.fTerrainType;
                const auto& info = eTerrsTexturesData::get(terrType);
                bool wall_;
                bool doors;
                bool open;
                uint8_t type;
                eTile::decodeWall(encoded, wall_, doors, open, type);
                const std::vector<eWallTexture>* types = nullptr;
                switch(wallType) {
                case eWallType::topLeft:
                    types = doors ? &info.fTLDoorsOpen :
                                &info.fTLWalls;
                    break;
                case eWallType::topRight:
                    types = doors ? &info.fTRDoorsOpen :
                                &info.fTRWalls;
                    break;
                }

                const int nTypes = types->size();
                if(nTypes <= type) return;
                const auto& wtex = (*types)[type];
                const int texId = wtex.fId;

                const auto& texs = eTerrsTextures::get(terrType);
                const auto& tex = texs.getTexture(texId);
                if(info.fWallsShadow) {
                    const float wallMin = open ? wtex.fWallMin : 0.f;
                    const float wallMax = open ? wtex.fWallMax : 1.f;
                    mGamePainter.addWallShadow(x, y, wallType, wallMin, wallMax);
                }

                renderElements.emplace_back(eRenderElement{eRenderElementType::wall,
                                                           std::static_pointer_cast<ePositioned>(wall)});

            };
            if(tile.fWallTL) addWall(eWallType::topLeft, tile.fWallTL);
            if(tile.fWallTR) addWall(eWallType::topRight, tile.fWallTR);
        };

        mTileIterator.nextIteration(this);
        bool iniObjs = true;
        for(const auto terrType : terrTypes) {
            if(terrType == 0) continue;
            const auto& texs = eTerrsTextures::get(terrType);
            mTileIterator.iterate([&](const eTileInfo& info) {
                const int x = info.fTX;
                const int y = info.fTY;
                if(iniObjs) handleTile(info);
                const auto& tile = mMap->tile(x, y);
                if(tile.fTerrainType != terrType) return;
                const auto tileType = tile.fTileType;
                if(tileType == 0) return;
                const auto& tex = texs.getTexture(tileType);
                const int px = info.fPX;
                const int py = info.fPY;
                mGamePainter.drawTexture(px, py + tileH, tex,
                                         eAlignment::top | eAlignment::hcenter);
            });
            iniObjs = false;
        }

        if(iniObjs) {
            mTileIterator.iterate([&](const eTileInfo& info) {
                handleTile(info);
            });
            iniObjs = false;
        }

        const auto window = eWidget::window();
        const bool altPressed = window->altPressed();
        mItemNames.clear();

        const float mult = res.multiplier();
        const int margin = 100*mult;
        const int w = width();
        const int h = height();
        for(const auto& i : mWorld.groundItems()) {
            const auto& pos = i->fPos;
            const auto pixel = tilePosToPixel(pos);
            if(pixel.fX < -margin || pixel.fY < -margin ||
               pixel.fX > w + margin || pixel.fY > h + margin) continue;
            const auto ipos = pos.floor();
            const auto tile = mTileIterator.getTile(ipos.fX, ipos.fY);
            if(!tile) continue;
            renderElements.emplace_back(eRenderElement{eRenderElementType::item,
                                                       std::static_pointer_cast<ePositioned>(i)});
        }
        for(const auto& u : mWorld.units()) {
            const auto& pos = u->fPos;
            const auto pixel = tilePosToPixel(pos);
            if(pixel.fX < -margin || pixel.fY < -margin ||
               pixel.fX > w + margin || pixel.fY > h + margin) continue;
            const auto ipos = pos.floor();
            const auto tile = mTileIterator.getTile(ipos.fX, ipos.fY);
            if(!tile) continue;
            renderElements.emplace_back(eRenderElement{eRenderElementType::unit,
                                                       std::static_pointer_cast<ePositioned>(u)});
        }
        {
            const auto& pos = mMainChar->fPos;
            mGamePainter.addLight(pos.fX, pos.fY, 3.f);
            const auto ipos = pos.floor();
            const auto tile = mTileIterator.getTile(ipos.fX, ipos.fY);
            if(tile) {
                renderElements.emplace_back(eRenderElement{eRenderElementType::unit,
                                                           std::static_pointer_cast<ePositioned>(mMainChar)});
            }
        }
        for(const auto& m : mWorld.missiles()) {
            const auto& pos = m->fPos;
            const auto pixel = tilePosToPixel(pos);
            if(pixel.fX < -margin || pixel.fY < -margin ||
               pixel.fX > w + margin || pixel.fY > h + margin) continue;
            const auto ipos = pos.floor();
            const auto tile = mTileIterator.getTile(ipos.fX, ipos.fY);
            if(!tile) continue;
            const auto missileType = m->fType;
            auto& missileTex = eMissilesTextures::sMissiles.get(missileType);
            const int appearId = missileTex.appearAnimId();
            const int baseId = missileTex.baseAnimId();
            const int hitId = missileTex.hitAnimId();
            int& frame = m->fFrame;
            int& animId = m->fAnimId;
            int nFrames = missileTex.nFrames(animId);
            if(frame >= nFrames) {
                if(animId == appearId) {
                    animId = baseId;
                    frame = 0;
                    nFrames = missileTex.nFrames(animId);
                } else if(animId == hitId) {
                    mWorld.removeMissile(*m);
                    continue;
                }
            }
            const int dirs = missileTex.nDirs(animId);
            const float ainc = 360.f/dirs;
            int dir = std::round(m->fAngle/ainc) + 2*dirs/16;
            dir = (dirs + dir) % dirs;
            const int texFrame = frame++ % nFrames;
            const auto& ftex = missileTex.get(animId, dir, texFrame);
            const float lradius = missileTex.lighting();
            if(lradius > 0.01f) {
                mGamePainter.addLight(pos.fX, pos.fY, lradius);
            }
            renderElements.emplace_back(eRenderElement{eRenderElementType::missile,
                                                       std::static_pointer_cast<ePositioned>(m),
                                                       ftex});
        }
        for(auto& n : mWorld.novas()) {
            const auto& c = n->fCenter;
            const float r = n->fRadius;
            const int nMissiles = 180;
            const eVec2f displ{r, 0.f};
            const float mangle = 180.f;
            const float dangle = 360.f/nMissiles;
            const auto& intervals = n->fIntervals;

            const auto missileType = n->fMissileType;
            auto& missileTex = eMissilesTextures::sMissiles.get(missileType);
            const int appearId = missileTex.appearAnimId();
            const int baseId = missileTex.baseAnimId();
            const int hitId = missileTex.hitAnimId();
            const int hitNFrames = hitId < 0 ? 0 : missileTex.nFrames(hitId);

            int frame = n->fFrame;
            int animId = appearId;
            const int nFrames = appearId < 0 ? 0 :
                missileTex.nFrames(appearId);
            if(frame >= nFrames) {
                animId = baseId;
                frame -= nFrames;
            }

            bool& ini = n->fInitialized;
            if(!ini) {
                ini = true;
                for(int i = 0; i < nMissiles; i++) {
                    const auto m = std::make_shared<eExtendedMissile>();
                    m->fAngle = mangle + i*dangle;
                    m->fType = missileType;
                    n->fMissiles.emplace_back(m);
                }
            }

            for(const auto& m : n->fMissiles) {
                if(m->fHit) continue;
                auto rdispl = displ;
                const float angle = m->fAngle - mangle;
                rdispl.rotate(angle);
                const ePointF pos = c + rdispl;
                const bool in = intervals.angleInRange(angle);
                const auto pixel = tilePosToPixel(pos);
                if(pixel.fX < -margin || pixel.fY < -margin ||
                   pixel.fX > w + margin || pixel.fY > h + margin) continue;
                if(in) {
                    m->fPos = pos;
                    m->fAnimId = animId;
                    m->fFrame = frame;
                } else {
                    int& animId = m->fAnimId;
                    int& frame = m->fFrame;
                    if(animId != hitId) {
                        animId = hitId;
                        frame = 0;
                    } else {
                        frame++;
                    }
                    if(frame + 1 >= hitNFrames) {
                        m->fHit = true;
                        if(animId < 0) continue;
                    }
                }
                const auto ipos = pos.floor();
                const auto tile = mTileIterator.getTile(ipos.fX, ipos.fY);
                if(tile) {
                    const int dirs = missileTex.nDirs(animId);
                    const float ainc = 360.f/dirs;
                    int dir = std::round(m->fAngle/ainc) + 2*dirs/16;
                    dir = (dirs + dir) % dirs;
                    const int texFrame = frame++ % nFrames;
                    const auto& ftex = missileTex.get(animId, dir, texFrame);
                    const float lradius = missileTex.lighting();
                    if(lradius > 0.01f) {
                        mGamePainter.addLight(pos.fX, pos.fY, lradius);
                    }
                    renderElements.emplace_back(eRenderElement{eRenderElementType::missile,
                                                               std::static_pointer_cast<ePositioned>(m),
                                                               ftex});
                }
            }
            n->fFrame++;
        }

        const auto typePriority = [](const eRenderElementType t) {
            switch(t) {
            case eRenderElementType::item:    return 0;
            case eRenderElementType::wall:    return 1;
            case eRenderElementType::object:  return 2;
            case eRenderElementType::unit:    return 3;
            case eRenderElementType::missile: return 4;
            }
            return 0;
        };

        std::sort(renderElements.begin(), renderElements.end(),
                  [&](const eRenderElement& e1,
                      const eRenderElement& e2) {
            const auto& u1 = e1.fPtr;
            const auto& u2 = e2.fPtr;
            if(!u1 && !u2) return false;
            if(!u1) return true;
            if(!u2) return false;

            const auto& p1 = u1->fPos;
            const auto& p2 = u2->fPos;

            const auto ip1 = p1.floor();
            const auto ip2 = p2.floor();

            if(ip1.fY != ip2.fY) return ip1.fY < ip2.fY;
            if(ip1.fX != ip2.fX) return ip1.fX < ip2.fX;

            if(p1.fY != p2.fY) return p1.fY < p2.fY;
            if(p1.fX != p2.fX) return p1.fX < p2.fX;
            return typePriority(e1.fType) < typePriority(e2.fType);
        });

        setHighlightedUnit(nullptr);
        setHighlightedObject(nullptr);
        setHighlightedItem(nullptr);
        setHighlightedDoors(std::nullopt);
        if(const auto p = mPressedUnit.lock()) {
            if(p->fHealth <= 0) {
                setPressedUnit(nullptr);
            }
        }

        mGamePainter.calculateAndRenderLighting();

        for(const auto& e : renderElements) {
            const auto& ePtr = e.fPtr;
            if(!ePtr) continue;
            const auto& pos = ePtr->fPos;
            const auto iPos = pos.floor();
            const auto pixel = tilePosToPixel(pos);
            const auto ipixel = pixel.round();
            if(e.fType == eRenderElementType::unit) {
                const auto u = std::static_pointer_cast<eUnit>(ePtr);
                auto& model = u->model();
                model.incFrame(by);
                bool highlight = false;
                if(!mHighlightUnit.lock() && u != mMainChar &&
                    (u->fHealth > 0 || u->isBody())) {
                    const SDL_Point p{int(mpos.fX), int(mpos.fY)};
                    const int w = 0.75*u->fRadius*tileW;
                    const int h = 2*w;
                    const SDL_Rect rect{ipixel.fX - w/2, ipixel.fY - h, w, h};
                    highlight = SDL_PointInRect(&p, &rect);
                    if(highlight) {
                        const auto b = model.requestBoundingRect();
                        const SDL_Rect rect{ipixel.fX + b.x, ipixel.fY + b.y, b.w, b.h};
                        highlight = SDL_PointInRect(&p, &rect);
                        if(highlight) {
                            setHighlightedUnit(u);
                            mHighlightObject.reset();
                            mHighlightItem.reset();
                            mHighlightDoors = std::nullopt;
                        }
                    }
                }
                if(const auto p = mPressedUnit.lock()) {
                    highlight = p == u;
                }
                SDL_Color colorMod{0, 0, 0, 0};
                const bool cold = u->cold();
                const bool poisoned = u->poisoned();
                if(cold) colorMod = SDL_Color{0, 128, 255, 255};
                else if(poisoned) colorMod = SDL_Color{0, 255, 55, 255};
                const auto tex = model.requestTexture(r);
                if(!tex) continue;
                const auto rect = model.requestBoundingRect();
                const int drawX = ipixel.fX + rect.x;
                const int drawY = ipixel.fY + rect.y;
                const eRenderCall c(eRenderCallType::unit, pos.fX, pos.fY,
                                    drawX, drawY, tex, highlight, true);
                mGamePainter.render(c);
            } else if(e.fType == eRenderElementType::missile) {
                const auto& ftex = e.fTex;
                const eRenderCall c(eRenderCallType::missile, pos.fX, pos.fY,
                                    pixel.fX, pixel.fY, ftex, false, false);
                mGamePainter.render(c);
            } else if(e.fType == eRenderElementType::item) {
                const auto i = std::static_pointer_cast<eGroundItem>(ePtr);
                const auto dataId = i->fDataId;
                auto& itex = eItemsTextures::getByItemDataId(dataId);
                itex.request(r, res);
                const auto& tex = itex.fTinyTex;
                bool highlight = false;
                const int texW = tex->width();
                const int texH = tex->height();
                int drawX = ipixel.fX;
                int drawY = ipixel.fY;
                ePainter::drawCoordinates(drawX, drawY, texW, texH, eAlignment::center);
                if(!mHighlightUnit.lock() && !mHighlightObject.lock() && !mHighlightItem.lock()) {
                    const SDL_Point p{int(mpos.fX), int(mpos.fY)};
                    const SDL_Rect rect{drawX, drawY, texW, texH};
                    const bool r = SDL_PointInRect(&p, &rect);
                    if(r) {
                        setHighlightedItem(i);
                        highlight = true;
                    }
                }
                mGamePainter.drawTexture(drawX, drawY, tex);
                if(highlight) {
                    tex->setBlendMode(SDL_BLENDMODE_ADD);
                    tex->setAlpha(125);
                    p.drawTexture(drawX, drawY, tex);
                    tex->setBlendMode(SDL_BLENDMODE_BLEND);
                    tex->clearAlphaMod();
                }
            } else if(e.fType == eRenderElementType::object) {
                const auto objPtr = std::static_pointer_cast<eObject>(ePtr);
                const auto& obj = *objPtr;
                const auto objType = obj.fObjectType;
                const auto& object = eObjectsInfo::sObjects.get(objType);
                const auto texObjectId = object.fTexId;
                const auto& objectTex = eObjsTextures::get(texObjectId);
                const auto& types = objectTex.fTypes;
                const auto typeId = obj.fSubtype % types.size();
                const auto& type = types[typeId];
                const auto& tex = type[obj.fState].fTexs.getTexture(0);
                const int h = object.fSize*tileH;
                const int dx = ((pos.fX - iPos.fX) - (pos.fY - iPos.fY))*(tileW/2);
                const int dy = ((pos.fX - iPos.fX) + (pos.fY - iPos.fY))*((tileH + 1)/2);
                const int x = ipixel.fX + dx;
                const int y = ipixel.fY + dy + h;

                bool highlight = false;
                const int texW = tex->width();
                const int texH = tex->height();
                int drawX = x;
                int drawY = y;
                ePainter::drawCoordinates(drawX, drawY, texW, texH,
                                          eAlignment::top | eAlignment::hcenter);
                if(!mHighlightUnit.lock() && !mHighlightObject.lock()) {
                    const SDL_Point p{int(mpos.fX), int(mpos.fY)};
                    const SDL_Rect rect{drawX, drawY, texW, texH};
                    const bool r = SDL_PointInRect(&p, &rect);
                    if(r) {
                        const auto type = obj.fObjectType;
                        const auto& info = eObjectsInfo::sObjects.get(type);
                        switch(info.fType) {
                        case eObjectType::none:
                            break;
                        case eObjectType::treasure: {
                            if(obj.fState == 0) {
                                setHighlightedObject(objPtr);
                                mHighlightItem.reset();
                                highlight = true;
                            }
                        } break;
                        };

                    }
                }
                const eRenderCall c(eRenderCallType::object,
                                    pos.fX + obj.fSize,
                                    pos.fY + obj.fSize,
                                    drawX, drawY, tex,
                                    highlight, true);
                mGamePainter.render(c);
            } else if(e.fType == eRenderElementType::wall) {
                const auto& wall = static_cast<eWall&>(*ePtr);
                const auto terrType = wall.fTerrainType;
                const auto& info = eTerrsTexturesData::get(terrType);
                const uint8_t encoded = wall.fEncoded;
                bool wall_;
                bool doors;
                bool open;
                uint8_t type;
                eTile::decodeWall(encoded, wall_, doors, open, type);
                const std::vector<eWallTexture>* types = nullptr;
                switch(wall.fType) {
                case eWallType::topLeft:
                    types = doors ? &info.fTLDoorsOpen :
                                    &info.fTLWalls;
                    break;
                case eWallType::topRight:
                    types = doors ? &info.fTRDoorsOpen :
                                    &info.fTRWalls;
                    break;
                }

                const int nTypes = types->size();
                if(nTypes <= type) continue;
                const auto& wtex = (*types)[type];
                const int texId = wtex.fId;

                const auto& texs = eTerrsTextures::get(terrType);
                const auto& tex = texs.getTexture(texId);

                const int bottomY = ipixel.fY + tileH;

                bool transparent = false;

                if(info.fWallsTransparent) {
                    int wallMaxTX_tmp;
                    switch(wall.fType) {
                    case eWallType::topLeft:
                        wallMaxTX_tmp = wallMaxTX;
                        break;
                    case eWallType::topRight:
                        wallMaxTX_tmp = wallMaxTX - 1;
                        break;
                    }
                    const SDL_Rect transRect{wallMinTX, wallMinTY,
                                             wallMaxTX_tmp - wallMinTX + 1,
                                             wallMaxTY - wallMinTY + 1};
                    const SDL_Point pt{iPos.fX, iPos.fY};
                    if(SDL_PointInRect(&pt, &transRect)) {
                        switch(wall.fType) {
                        case eWallType::topLeft:
                            transparent = uipos.fX < iPos.fX;
                            break;
                        case eWallType::topRight:
                            transparent = uipos.fY < iPos.fY;
                            break;
                        }
                    }
                }

                const int texW = tex->width();
                const int texH = tex->height();
                int drawX = ipixel.fX;
                int drawY = bottomY;
                ePainter::drawCoordinates(drawX, drawY, texW, texH,
                                          eAlignment::top | eAlignment::hcenter);

                const eRenderCall c(eRenderCallType::wall,
                                    pos.fX, pos.fY,
                                    drawX, drawY, tex,
                                    false, false,
                                    wall.fType,
                                    transparent);
                mGamePainter.render(c);
                bool highlight = false;
                if(doors) {
                    if(!mHighlightUnit.lock() && !mHighlightObject.lock() &&
                        mHighlightDoors == std::nullopt) {
                        const SDL_Point p{int(mpos.fX), int(mpos.fY)};
                        const int w = tex->width();
                        const int h = tex->height();
                        const SDL_Rect rect{ipixel.fX - w/2, ipixel.fY - h, w, h};
                        const bool partHighlight = SDL_PointInRect(&p, &rect);
                        eDoors doors;
                        doors.fOpen = open;
                        doors.fType = wall.fType;
                        auto& tiles = doors.fTiles;
                        switch(wall.fType) {
                        case eWallType::topLeft: {
                            for(int dy = -type; dy < nTypes - type; dy++) {
                                const eDoorsTile tile{iPos.fX, iPos.fY + dy};
                                tiles.emplace_back(tile);
                            }
                        } break;
                        case eWallType::topRight: {
                            for(int dx = -type; dx < nTypes - type; dx++) {
                                const eDoorsTile tile{iPos.fX + dx, iPos.fY};
                                tiles.emplace_back(tile);
                            }
                        } break;
                        }
                        if(partHighlight) {
                            highlight = true;
                            setHighlightedDoors(doors);
                            mHighlightObject.reset();
                            mHighlightItem.reset();
                        } else {
                            for(const auto& t : tiles) {
                                if(iPos.fX == t.fX && iPos.fY == t.fY) {
                                    highlight = true;
                                    break;
                                }
                            }
                        }
                    }
                }

                if(doors && !open) {
                    const std::vector<eWallTexture>* types = nullptr;
                    switch(wall.fType) {
                    case eWallType::topLeft:
                        types = &info.fTLDoors;
                        break;
                    case eWallType::topRight:
                        types = &info.fTRDoors;
                        break;
                    }

                    const int nTypes = types->size();
                    if(nTypes > type) {
                        const int texId = (*types)[type].fId;
                        const auto& tex = texs.getTexture(texId);
                        if(transparent) tex->setAlpha(128);
                        mGamePainter.drawTexture(ipixel.fX, bottomY, tex,
                                                 eAlignment::top | eAlignment::hcenter);
                        if(transparent) tex->clearAlphaMod();

                        if(highlight) {
                            tex->setBlendMode(SDL_BLENDMODE_ADD);
                            tex->setAlpha(125);
                            mGamePainter.drawTexture(ipixel.fX, bottomY, tex,
                                                     eAlignment::top | eAlignment::hcenter);
                            tex->setBlendMode(SDL_BLENDMODE_BLEND);
                            tex->clearAlphaMod();
                        }
                    }
                }
            }
        }

        if(altPressed) {
            std::map<float, eGroundItem*> items;
            for(const auto& i : mWorld.groundItems()) {
                const float dist = ePointF::distance(i->fPos, mMainChar->fPos);
                items[dist] = i.get();
            }
            for(const auto& it : items) {
                const auto& i = it.second;
                const auto& pos = i->fPos;
                const auto pixel = tilePosToPixel(pos);
                const auto ipixel = pixel.floor();
                mItemNames.add(ipixel, *i);
            }
        }
        if(altPressed || mHighlightItem.lock()) {
            const SDL_Point impos{int(mpos.fX), int(mpos.fY)};
            const auto holder = mGamePainter.switchToItemNames();
            for(const auto& it : mItemNames) {
                const auto& item = it.second;
                const auto& rect = item.fRect;
                SDL_Color color;
                if(SDL_PointInRect(&impos, &rect)) {
                    color = SDL_Color{0, 0, 255, 200};
                } else {
                    color = SDL_Color{0, 0, 0, 150};
                }
                mGamePainter.fillRect(rect, color);
                mGamePainter.drawTexture(rect, item.fName,
                                         eAlignment::center);
            }
        }
    }

    mGamePainter.finish(res);

    const auto& tex = texture();
    if(tex) {
        p.drawTexture(rect(), tex, eAlignment::center);
    }

    const int m = res.smallPadding();
    int y = m;
    for(int i = 0; i < mMessages.size(); i++) {
        auto& msg = mMessages[i];
        if(msg.fFramesRemaining-- <= 0) {
            mMessages.erase(mMessages.begin() + i);
            i--;
        } else {
            p.drawTexture(m, y, msg.fTex);
            y += msg.fTex->height() + m;
        }
    }
}

bool eGameWidget::mousePressEvent(const eMouseEvent& e) {
    const auto button = e.button();
    const bool leftPressed = static_cast<bool>(
        button & eMouseButton::left);
    const bool rightPressed = static_cast<bool>(
        button & eMouseButton::right);
    if(leftPressed || rightPressed) {
        mMainAction.mousePress();
        if(e.altPreseed()) {
            uint32_t itemId;
            const bool r = mItemNames.at({e.x(), e.y()}, itemId);
            if(r) {
                const auto item = mWorld.getItem(itemId);
                if(item) {
                    mMainAction.setPressedItem(item);
                }
            }
        } else if(const auto h = mHighlightUnit.lock()) {
            setPressedUnit(h);
        } else if(const auto o = mHighlightObject.lock()) {
            mMainAction.setPressedObject(o);
        } else if(const auto i = mHighlightItem.lock()) {
            mMainAction.setPressedItem(i);
        } else if(const auto d = mHighlightDoors) {
            mMainAction.setPressedDoors(d);
        }
        mInput.handleMousePress(leftPressed, rightPressed,
                                float(e.x()), float(e.y()));
    }
    return true;
}

bool eGameWidget::mouseReleaseEvent(const eMouseEvent& e) {
    const auto button = e.button();
    const bool leftReleased = static_cast<bool>(
        button & eMouseButton::left);
    const bool rightRelease = static_cast<bool>(
        button & eMouseButton::right);
    if(leftReleased || rightRelease) {
        mInput.handleMouseRelease(leftReleased, rightRelease);
        const auto schoice = leftReleased ? eSkillChoice::left :
                                 eSkillChoice::right;
        const bool rangeAttack = mMainAction.rangedAttack(schoice);
        if(e.shiftPressed() || (rightRelease && rangeAttack) ||
           (rangeAttack && mPressedUnit.lock())) {
            mMainAction.stop();
        } else {
            const auto pos = pixelToTilePos(mInput.mousePos());
            mMainAction.mouseRelease(pos);
        }
        setPressedUnit(nullptr);
    }
    return true;
}

bool eGameWidget::mouseMoveEvent(const eMouseEvent& e) {
    mInput.handleMouseMove(float(e.x()), float(e.y()));
    return true;
}

void eGameWidget::initializeTextures() {
    const int w = width();
    const int h = height();
    const int tw = mInput.tileWidth();
    const int th = mInput.tileHeight();
    const auto tex = mGamePainter.initialize(w, h, tw, th);
    setTexture(tex);
}

void eGameWidget::setHighlightedUnit(const std::shared_ptr<eUnit>& u) {
    mHighlightUnit = u;
    if(mUnitIndicator && !mPressedUnit.lock()) {
        mUnitIndicator->setUnit(u, mUserNames);
    }
}

void eGameWidget::setHighlightedObject(const std::shared_ptr<eObject>& obj) {
    mHighlightObject = obj;
    if(obj) {
        const auto type = obj->fObjectType;
        const auto name = eObjectNames::name(type);
        const auto& pos = obj->fPos;
        const float size = obj->fSize;
        const eVec2f d{size, size};
        const auto pixel = tilePosToPixel(pos - d);
        const auto ipixel = pixel.floor();
        const SDL_Rect rect{ipixel.fX, ipixel.fY, 0, 0};
        eHoverWidget::sSetGameTooltip(name, rect);
    } else {
        eHoverWidget::sSetGameTooltip("");
    }
}

void eGameWidget::setHighlightedDoors(const std::optional<eDoors>& doors) {
    mHighlightDoors = doors;
    if(doors != std::nullopt) {
        const auto pos = doors->pos();
        const auto pixel = tilePosToPixel(pos);
        const auto ipixel = pixel.floor();
        const auto& res = resolution();
        const float mult = res.multiplier();
        const int h = 100*mult;
        const SDL_Rect rect{ipixel.fX, ipixel.fY - h, 0, 0};
        const int s = doors->fOpen ? 1 : 0;
        const auto text = eLanguage::text(15, s);
        eHoverWidget::sSetGameTooltip(text, rect);
    } else {
        eHoverWidget::sSetGameTooltip("");
    }
}

void eGameWidget::setHighlightedItem(const std::shared_ptr<eGroundItem>& i) {
    mHighlightItem = i;
    if(i) {
        const auto& pos = i->fPos;
        const auto pixel = tilePosToPixel(pos);
        const auto ipixel = pixel.floor();
        mItemNames.add(ipixel, *i);
    }
}

void eGameWidget::setPressedUnit(const std::shared_ptr<eUnit>& u) {
    mPressedUnit = u;
    if(mUnitIndicator) {
        if(u) {
            mUnitIndicator->setUnit(u, mUserNames);
        } else {
            mUnitIndicator->setUnit(mHighlightUnit.lock(), mUserNames);
        }
    }

    if(u) mMainAction.setPressedUnit(u);
}

void eGameWidget::addMessage(SDL_Renderer* const r,
                             const std::string& text) {
    const auto& res = resolution();
    const auto font = eFonts::textFont(res.tinyFontSize());
    const int w = width()/2;
    auto& msg = mMessages.emplace_back();
    msg.fText = text;
    msg.fFramesRemaining = 5*text.size() + 250;
    eTextGenerator gen(r, eFontColor::white, font, 1, w);
    msg.fTex = gen.generate(text);
}

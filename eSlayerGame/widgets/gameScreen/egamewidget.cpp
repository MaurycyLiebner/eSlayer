#include "egamewidget.h"

#include "../../emainwindow.h"

#include "../../textures/eobjstextures.h"
#include "../../textures/eterrstextures.h"
#include "../../textures/etilesiterator.h"
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
    mGamePainter(renderer()) {
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
        };

        struct eWall : public ePositioned {
            int fTerrainType;
            eWallType fType;
        };

        std::vector<eRenderElement> renderElements;

        int wallMinTX = uipos.fX - 1000;
        int wallMaxTX = uipos.fX + 1000;
        int wallMinTY = uipos.fY - 1000;
        int wallMaxTY = uipos.fY + 1000;
        const auto handleTile = [&](const int x, const int y) {
            const auto& iobjs = mMap->objects(x, y);
            for(const auto& iobj : iobjs) {
                const auto& obj = mMap->object(iobj);
                renderElements.emplace_back(eRenderElement{eRenderElementType::object,
                                                           std::static_pointer_cast<ePositioned>(obj)});
            }
            const auto& tile = mMap->tile(x, y);
            const auto addWall = [&](const eWallType type) {
                const auto wall = std::make_shared<eWall>();
                wall->fPos = ePointF{float(x), float(y)};
                wall->fTerrainType = tile.fTerrainType;
                wall->fType = type;
                switch(type) {
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

                renderElements.emplace_back(eRenderElement{eRenderElementType::wall,
                                                           std::static_pointer_cast<ePositioned>(wall)});

            };
            if(tile.fWallTL) addWall(eWallType::topLeft);
            if(tile.fWallTR) addWall(eWallType::topRight);
        };

        eTilesIterator iterator;
        iterator.initialize(this);
        bool iniObjs = true;
        for(const auto terrType : terrTypes) {
            const auto& texs = eTerrsTextures::get(terrType);
            iterator.iterate([&](const int x, const int y) {
                if(iniObjs) handleTile(x, y);
                const auto& tile = mMap->tile(x, y);
                if(tile.fTerrainType != terrType) return;
                const auto tileType = tile.fTileType;
                if(tileType != 0) {
                    const auto pos = ePointF(x, y);
                    const auto pixel = tilePosToPixel(pos);
                    const auto ipixel = pixel.round();
                    const auto& tex = texs.getTexture(tileType);
                    mGamePainter.drawTexture(ipixel.fX, ipixel.fY + tileH, tex,
                                             eAlignment::top | eAlignment::hcenter);
                }
            });
            iniObjs = false;
        }

        if(iniObjs) {
            iterator.iterate([&](const int x, const int y) {
                handleTile(x, y);
            });
            iniObjs = false;
        }

        const auto window = eWidget::window();
        const bool altPressed = window->altPressed();
        mItemNames.clear();
        const auto& res = resolution();
        const int fontSize = res.smallFontSize();
        const auto font = eFonts::textFont(fontSize);

        const float mult = res.multiplier();
        const int margin = 100*mult;
        const int w = width();
        const int h = height();
        for(const auto& i : mWorld.groundItems()) {
            const auto pixel = tilePosToPixel(i->fPos);
            if(pixel.fX < -margin || pixel.fY < -margin ||
               pixel.fX > w + margin || pixel.fY > h + margin) continue;
            renderElements.emplace_back(eRenderElement{eRenderElementType::item,
                                                       std::static_pointer_cast<ePositioned>(i)});
        }
        for(const auto& u : mWorld.units()) {
            const auto pixel = tilePosToPixel(u->fPos);
            if(pixel.fX < -margin || pixel.fY < -margin ||
               pixel.fX > w + margin || pixel.fY > h + margin) continue;
            renderElements.emplace_back(eRenderElement{eRenderElementType::unit,
                                        std::static_pointer_cast<ePositioned>(u)});
        }
        renderElements.emplace_back(eRenderElement{eRenderElementType::unit,
                                    std::static_pointer_cast<ePositioned>(mMainChar)});
        for(const auto& m : mWorld.missiles()) {
            const auto pixel = tilePosToPixel(m->fPos);
            if(pixel.fX < -margin || pixel.fY < -margin ||
               pixel.fX > w + margin || pixel.fY > h + margin) continue;
            renderElements.emplace_back(eRenderElement{eRenderElementType::missile,
                                        std::static_pointer_cast<ePositioned>(m)});
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
        if(const auto p = mPressedUnit.lock()) {
            if(p->fHealth <= 0) {
                setPressedUnit(nullptr);
            }
        }

        for(const auto& e : renderElements) {
            const auto& ePtr = e.fPtr;
            if(!ePtr) continue;
            const auto& pos = ePtr->fPos;
            const auto iPos = pos.floor();
            const auto pixel = tilePosToPixel(pos);
            const auto ipixel = pixel.round();
            if(e.fType == eRenderElementType::unit) {
                const auto u = std::static_pointer_cast<eUnit>(ePtr);
                mGamePainter.save();
                mGamePainter.translate(ipixel.fX, ipixel.fY);
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
                        const auto b = model.offsetBoundingRect();
                        const SDL_Rect rect{ipixel.fX + b.x, ipixel.fY + b.y, b.w, b.h};
                        highlight = SDL_PointInRect(&p, &rect);
                        if(highlight) {
                            setHighlightedUnit(u);
                            mHighlightObject.reset();
                        }
                    }
                }
                if(const auto p = mPressedUnit.lock()) {
                    highlight = p == u;
                }
                model.draw(mGamePainter, res, highlight);
                const bool drawLighting = u == mMainChar;
                if(drawLighting) {
                    auto paintCall = model.paintCall(r);
                    paintCall.fX += mGamePainter.x();
                    paintCall.fY += mGamePainter.y();
                    mGamePainter.renderLight(pos.fX, pos.fY,
                                             pixel.fX, pixel.fY, 10.f,
                                             SDL_Color{255, 255, 255, 255},
                                             paintCall);
                }
                mGamePainter.restore();
            } else if(e.fType == eRenderElementType::missile) {
                const auto m = std::static_pointer_cast<eExtendedMissile>(ePtr);
                const auto missileType = m->fType;
                auto& missileTex = eMissilesTextures::sMissiles.get(missileType);
                const int dirs = missileTex.nDirs(0);
                const float ainc = 360.f/dirs;
                int dir = std::round(m->fAngle/ainc) + 2*dirs/16;
                dir = (dirs + dir) % dirs;
                const auto& ftex = missileTex.get(0, dir, mFrame % missileTex.nFrames(0));
                const float lradius = missileTex.lighting();
                if(lradius > 0.01f) {
                    const ePaintCall paintCall{pixel.fX, pixel.fY, ftex};
                    mGamePainter.renderLight(pos.fX, pos.fY,
                                             pixel.fX, pixel.fY,
                                             lradius, SDL_Color{255, 255, 255, 255},
                                             paintCall);
                }
                mGamePainter.drawTexture(pixel.fX, pixel.fY, ftex);
            } else if(e.fType == eRenderElementType::item) {
                const auto i = std::static_pointer_cast<eGroundItem>(ePtr);
                const auto dataId = i->fDataId;
                auto& itex = eItemsTextures::getByItemDataId(dataId);
                itex.request(r, res);
                mGamePainter.drawTexture(pixel.fX, pixel.fY, itex.fTinyTex,
                                         eAlignment::center);
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
                const auto& tex = type[0].fTexs.getTexture(0);
                const int h = object.fSize*tileH;
                const auto& pos = obj.fPos;
                const float fpx = pixel.fX + 0.5f*((pos.fX - iPos.fX) - (pos.fY - iPos.fY))*tileW;
                const float fpy = pixel.fY + 0.5f*((pos.fX - iPos.fX) + (pos.fY - iPos.fY))*tileH;
                if(eRenderSettings::sRenderObjectShadows && objectTex.fBlocksLight) {
                    mGamePainter.addObjectShadow(fpx, fpy + h, fpy + 0.5f*h,
                                                 object.fSize, tex);
                }
                bool highlight = false;
                const int texW = tex->width();
                const int texH = tex->height();
                const int x = fpx;
                const int y = fpy + h;
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
                        case eObjectType::treasure:
                            setHighlightedObject(objPtr);
                            highlight = true;
                            break;
                        };

                    }
                }
                mGamePainter.drawShadow(x - texW/2, y, *tex);
                mGamePainter.drawTexture(drawX, drawY, tex);
                if(highlight) {
                    tex->setBlendMode(SDL_BLENDMODE_ADD);
                    tex->setAlpha(125);
                    p.drawTexture(drawX, drawY, tex);
                    tex->setBlendMode(SDL_BLENDMODE_BLEND);
                    tex->clearAlphaMod();
                }
            } else if(e.fType == eRenderElementType::wall) {
                const auto& wall = static_cast<eWall&>(*ePtr);
                const auto terrType = wall.fTerrainType;
                const auto& info = eTerrsTexturesData::get(terrType);
                const std::vector<int>* types = nullptr;
                switch(wall.fType) {
                case eWallType::topLeft:
                    types = &info.fTLWalls;
                    break;
                case eWallType::topRight:
                    types = &info.fTRWalls;
                    break;
                }

                if(!types || types->empty()) continue;
                const int id = (iPos.fX + iPos.fY) % types->size();
                const int texId = (*types)[id];

                const auto& texs = eTerrsTextures::get(terrType);
                const auto& tex = texs.getTexture(texId);

                const int bottomY = ipixel.fY + tileH;

                if(eRenderSettings::sRenderWallShadows) {
                    mGamePainter.addWallShadow(iPos.fX, iPos.fY,
                                               pixel.fX, bottomY,
                                               wall.fType, tileW, tileH,
                                               tex);
                }
                bool transparent = false;

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

                if(transparent) tex->setAlpha(128);
                mGamePainter.drawTexture(ipixel.fX, bottomY, tex,
                                         eAlignment::top | eAlignment::hcenter);
                if(transparent) tex->clearAlphaMod();
            }
        }

        if(altPressed) {
            std::map<float, eGroundItem*> items;
            for(const auto& i : mWorld.groundItems()) {
                const float dist = ePointF::distance(i->fPos, mMainChar->fPos);
                items[dist] = i.get();
            }
            for(const auto& it : items) {
                const int w = width();
                const int h = height();
                const auto i = it.second;
                const auto pixel = tilePosToPixel(i->fPos);
                mItemNames.add(r, font, w, h, pixel.floor(), *i);
            }
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
    const auto tex = mGamePainter.initialize(w, h);
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

#include "egamewidget.h"

#include "../../emainwindow.h"

#include "../../textures/eobjstextures.h"
#include "../../textures/eterrstextures.h"
#include "../../textures/etilesiterator.h"
#include "../../textures/emissilestextures.h"
#include "../../textures/etextgenerator.h"
#include "eunitindicator.h"
#include "eitemdragwidget.h"
#include "einventorywidget.h"
#include "../../elanguage.h"

#include <eSlayerMissiles/emissileincrement.h>

#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/earea.h>
#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/estringhelpers.h>

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
                             const eCharacter& c) {
    mCName = c.name();
    mUserNames[clientId] = mCName;
    mHardcore = c.hardcore();

    mClientId = clientId;
    mServer = server;
    mMap = map;

    initializeTextures();

    const auto r = renderer();
    const auto& res = resolution();

    const auto w = walkable();
    const auto iter = [this](const ePointF& pos,
                             const float dist,
                             const eOtherHandler& handler) {
        for(const auto& u : mWorld.units()) {
            if(!u) continue;
            handler(*u);
        }
    };
    mMainAction.initialize(mServer, res, r, w, iter, clientId, 0);
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
    mServer->dropItem(mClientId, dragged.fItemId);
    dragged = eItem();
    eItemDragWidget::sUpdateDragItem(eq);
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
            eItemDragWidget::sUpdateDragItem(eq);
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

    const int tileH = eGameWidget::tileHeight();
    {
        const auto holder = mGamePainter.switchToBase();

        const auto& terrTypes = mMap->terrainTypes();
        const auto& objTypes = mMap->objectTypes();

        eTilesIterator iterator;
        iterator.initialize(this);
        for(const auto terrType : terrTypes) {
            if(terrType == 0) continue;
            const auto& texs = eTerrsTextures::get(terrType);
            iterator.iterate([&](const int x, const int y,
                                 const int px, const int py) {
                const auto& tile = mMap->tile(x, y);
                if(tile.fTerrainType != terrType) return;
                const auto& tex = texs.getTexture(tile.fTileType);
                mGamePainter.drawTexture(px, py + tileH, tex,
                                         eAlignment::top | eAlignment::hcenter);
            });
        }

        const auto window = eWidget::window();
        const bool altPressed = window->altPressed();
        mItemNames.clear();
        const auto& res = resolution();
        const int fontSize = res.smallFontSize();
        const auto font = eFonts::textFont(fontSize);

        enum class eRenderElementType {
            unit, missile, item
        };

        struct eRenderElement {
            eRenderElementType fType;
            std::shared_ptr<ePositioned> fPtr;
        };
        std::vector<eRenderElement> renderElements;
        const int margin = 100;
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
            case eRenderElementType::unit:    return 1;
            case eRenderElementType::missile: return 2;
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

            const int ty1 = ip1.fX + ip1.fY;
            const int tx1 = (ip1.fX + ip1.fY)/2 - ip1.fY;
            const int ty2 = ip2.fX + ip2.fY;
            const int tx2 = (ip2.fX + ip2.fY)/2 - ip2.fY;

            if(ty1 != ty2) return ty1 < ty2;
            if(tx1 != tx2) return tx1 < tx2;

            const auto pd1 = tilePosToPixel(p1);
            const auto pd2 = tilePosToPixel(p2);
            if(pd1.fY != pd2.fY) return pd1.fY < pd2.fY;
            if(pd1.fX != pd2.fX) return pd1.fX < pd2.fX;
            return typePriority(e1.fType) < typePriority(e2.fType);
        });

        int nextElement = 0;
        setHighlightedUnit(nullptr);
        if(const auto p = mPressedUnit.lock()) {
            if(p->fHealth <= 0) {
                setPressedUnit(nullptr);
            }
        }

        const int tileW = mInput.tileWidth();
        iterator.iterate([&](const int x, const int y,
                             const int px, const int py) {
            const auto& iobjs = mMap->objects(x, y);
            for(const auto& iobj : iobjs) {
                const auto& obj = mMap->object(iobj);
                const auto objType = obj.fObjectType;
                const auto& object = eObjsTextures::get(objType);
                const auto& tex = object.getTexture(obj.fTileType);
                p.drawTexture(px, py + tileH, tex, eAlignment::top | eAlignment::hcenter);
            }
            for(int eleId = nextElement; eleId < (int)renderElements.size(); eleId++) {
                const auto& e = renderElements[eleId];
                const auto& ePtr = e.fPtr;
                if(!ePtr) continue;
                const auto& pos = ePtr->fPos;
                const auto iPos = pos.floor();
                if(iPos.fY != y) continue;
                if(iPos.fX != x) continue;
                const auto pixel = tilePosToPixel(pos);
                if(e.fType == eRenderElementType::unit) {
                    const auto u = std::static_pointer_cast<eUnit>(ePtr);
                    mGamePainter.save();
                    const auto idispl = pixel.round();
                    mGamePainter.translate(idispl.fX, idispl.fY);
                    auto& model = u->model();
                    model.incFrame(by);
                    bool highlight = false;
                    if(!mHighlightUnit.lock() && u != mMainChar &&
                       (u->fHealth > 0 || u->isBody())) {
                        const SDL_Point p{int(mpos.fX), int(mpos.fY)};
                        const int w = 0.75*u->fRadius*tileW;
                        const int h = 2*w;
                        const SDL_Rect rect{idispl.fX - w/2, idispl.fY - h, w, h};
                        highlight = SDL_PointInRect(&p, &rect);
                        if(highlight) {
                            const auto b = model.offsetBoundingRect();
                            const SDL_Rect rect{idispl.fX + b.x, idispl.fY + b.y, b.w, b.h};
                            highlight = SDL_PointInRect(&p, &rect);
                            if(highlight) {
                                setHighlightedUnit(u);
                            }
                        }
                    }
                    if(const auto p = mPressedUnit.lock()) {
                        highlight = p == u;
                    }
                    model.draw(mGamePainter, res, highlight);
                    mGamePainter.restore();
                } else if(e.fType == eRenderElementType::missile) {
                    const auto m = std::static_pointer_cast<eExtendedMissile>(ePtr);
                    const auto missileType = m->fType;
                    auto& missileTex = eMissilesTextures::sMissiles.get(missileType);
                    const int dirs = missileTex.nDirs(0);
                    const float ainc = 360.f/dirs;
                    int dir = std::round(m->fAngle/ainc) + 2*dirs/16;
                    dir = (dirs + dir) % dirs;
                    const float lradius = missileTex.lighting();
                    if(lradius > 0.01f) {
                        mGamePainter.renderLight(r, pixel.fX, pixel.fY,
                                                 lradius, SDL_Color{255, 255, 255, 255});
                    }
                    const auto& ftex = missileTex.get(0, dir, mFrame % missileTex.nFrames(0));
                    mGamePainter.drawTexture(pixel.fX, pixel.fY, ftex);
                } else if(e.fType == eRenderElementType::item) {
                    const auto i = std::static_pointer_cast<eGroundItem>(ePtr);
                    mGamePainter.fillRect(SDL_Rect{int(pixel.fX) - 2, int(pixel.fY) - 2,
                                                   4, 4}, SDL_Color{255, 0, 0, 255});
                }
                nextElement = eleId + 1;
            }
        });

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

    mGamePainter.renderLight(r, mInput.characterHorizontalPos()*width(),
                             mInput.characterVerticalPos()*height(),
                             10.f, SDL_Color{255, 255, 255, 255});
    mGamePainter.finish();

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
        }
        mInput.handleMousePress(leftPressed, rightPressed,
                                float(e.x()), float(e.y()));
        if(const auto h = mHighlightUnit.lock()) {
            setPressedUnit(h);
        }
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
    mGamePainter.setLightness(mMap->lightness());
    mGamePainter.setContrast(mMap->contrast());
    setTexture(tex);
}

void eGameWidget::setHighlightedUnit(const std::shared_ptr<eUnit>& u) {
    mHighlightUnit = u;
    if(mUnitIndicator && !mPressedUnit.lock()) {
        mUnitIndicator->setUnit(u, mUserNames);
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

eWalkable eGameWidget::walkable() const {
    return [this](const int x, const int y) {
        return mMap->walkable(x, y);
    };
}

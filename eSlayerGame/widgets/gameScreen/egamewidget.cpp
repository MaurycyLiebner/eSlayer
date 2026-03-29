#include "egamewidget.h"

#include "../../emainwindow.h"

#include "../../textures/eobjstextures.h"
#include "../../textures/eterrstextures.h"
#include "../../textures/etilesiterator.h"
#include "../../textures/emissilestextures.h"
#include "eunitindicator.h"

#include <eSlayerMissiles/emissileincrement.h>

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eunitarea.h>
#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/eitemsdata.h>

eGameWidget::eGameWidget(eMainWindow* const window) :
    eLabel(window),
    mGamePainter(renderer()) {
    setNoPadding();
}

void eGameWidget::initialize(const int clientId,
                             const std::shared_ptr<eServer>& server,
                             const std::shared_ptr<eMap>& map,
                             const eEquipment& eq) {
    mClientId = clientId;
    mServer = server;
    mMap = map;

    initializeTextures();

    const auto r = renderer();

    const auto w = walkable();
    const auto iter = [this](const eOtherHandler& handler) {
        for(const auto& u : mWorld.units()) {
            if(!u) continue;
            handler(*u);
        }
    };
    mMainAction.initialize(mServer, r, w, iter, clientId, 0);
    mMainChar = mMainAction.unit();

    setRightSkill(0);
    setLeftSkill(0);
    mServer->requestWeaponData(mClientId);

    mEq = eq;
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

void eGameWidget::dropItem() {
    auto& dragged = mEq.fDragged;
    if(dragged.fType == eItemType::none) return;
    mServer->dropItem(mClientId, dragged.fItemId);
    dragged = eItem();
}

void eGameWidget::sendInventoryRearranged() {
    mServer->rearrangeItems(mClientId, mEq);
}

void eGameWidget::setLeftSkill(const int s) {
    if(mLeftSkill == s) return;
    mLeftSkill = s;
    mServer->setSkillId(mClientId, eSkillChoice::left, s);
}

void eGameWidget::setRightSkill(const int s) {
    if(mRightSkill == s) return;
    mRightSkill = s;
    mServer->setSkillId(mClientId, eSkillChoice::right, s);
}

void eGameWidget::respawn() {
    mServer->respawn(mClientId);
}

bool eGameWidget::switchRunning() {
    const bool run = !mMainAction.running();
    mMainAction.setRunning(run);
    return run;
}

bool eGameWidget::switchWeapons() {
    mEq.fWeapons1 = !mEq.fWeapons1;
    return mEq.fWeapons1;
}

void eGameWidget::disconnect() {
    if(mServer) {
        mServer->disconnect(mClientId);
    }
}

void eGameWidget::paintEvent(ePainter& p) {
    mGamePainter.clear();

    const float by = 1.f;
    mServer->increment(by);

    const auto r = renderer();

    const auto worldResult = mWorld.processServerData(
        mClientId, *mServer, *mMainChar, mMainAction, r);

    if(worldResult.fReceived) {
        if(worldResult.fHasMainCharData) {
            const auto& u = worldResult.fMainCharData;
            if(mMainChar->fHealth <= 0 && u.fHealth > 0) {
                mMainChar->fPos = u.fPos;
                mMainAction.setPos(u.fPos);
                mMainAction.stop();
                setPressedUnit(nullptr);
                setHighlightedUnit(nullptr);
                if(mRespawnHandler) mRespawnHandler();
            }
            mMainChar->fHealth = u.fHealth;
            if(u.fHealth <= 0) {
                if(mDeathHandler) mDeathHandler();
                mMainAction.stop();
            }
            mMainChar->fMaxHealth = u.fMaxHealth;
            mMainChar->fActionTime = u.fActionTime;
            if(u.fAnimId > mMainChar->fAnimId) {
                mMainChar->fAnim = u.fAnim;
                mMainChar->fAnimId = u.fAnimId;
                mMainChar->fAnimSpeed = u.fAnimSpeed;
            }
            if(mMainCharHandler) mMainCharHandler(u);
        }

        auto& model = mMainChar->model();
        model.setAggressive(worldResult.fAggressive);
    }

    mServer->changeState(mClientId, *mMainChar);

    mWorld.simulateMissiles(by, mMap);

    if(!mMenuVisible) {
        const auto mouseTilePos = pixelToTilePos(mInput.mousePos());
        const auto w = window();
        const bool shiftPressed = w->shiftPressed();
        mMainAction.increment(mInput.mousePressed(),
                              mInput.rightPressed(),
                              shiftPressed,
                              mouseTilePos,
                              mInput.rightPressed() ? mRightSkill :
                                                      mLeftSkill,
                              by);
    }

    mFrame++;

    const int tileH = eGameWidget::tileHeight();
    {
        const auto holder = mGamePainter.switchToBase();

        const auto& terrTypes = mMap->terrainTypes();
        const auto& objTypes = mMap->objectTypes();
        const int iMax = terrTypes.size() - 1;

        eTilesIterator iterator;
        iterator.initialize(this);
        for(int i = 0; i <= iMax; i++) {
            const auto& terrType = terrTypes[i];
            const auto floor = eTerrsTextures::get(terrType.fName);
            iterator.iterate([&](const int x, const int y,
                                 const int px, const int py) {
                const auto& tile = mMap->tile(x, y);
                if(tile.fTerrainType != i) return;
                const auto& tex = floor->getTexture(tile.fTileType);
                mGamePainter.drawTexture(px, py + tileH, tex, eAlignment::top | eAlignment::hcenter);
            });
        }

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
            return pd1.fX < pd2.fX;
        });

        int nextElement = 0;
        setHighlightedUnit(nullptr);
        if(mPressedUnit && mPressedUnit->fHealth <= 0) {
            setPressedUnit(nullptr);
        }
        const auto& mousePos = mInput.mousePos();
        const int tileW = mInput.tileWidth();
        iterator.iterate([&](const int x, const int y,
                             const int px, const int py) {
            const auto& iobjs = mMap->objects(x, y);
            for(const auto& iobj : iobjs) {
                const auto& obj = mMap->object(iobj);
                const auto& objType = objTypes[obj.fObjectType];
                const auto object = eObjsTextures::get(objType.fName);
                const auto& tex = object->getTexture(obj.fTileType);
                p.drawTexture(px, py + tileH, tex, eAlignment::top | eAlignment::hcenter);
            }
            for(int eleId = nextElement; eleId < (int)renderElements.size(); eleId++) {
                const auto& e = renderElements[eleId];
                if(e.fType == eRenderElementType::unit) {
                    const auto u = std::static_pointer_cast<eUnit>(e.fPtr);
                    if(!u) continue;
                    const auto& pos = u->fPos;
                    const auto iPos = pos.floor();
                    if(iPos.fY != y) continue;
                    if(iPos.fX != x) continue;
                    mGamePainter.save();
                    const auto displ = tilePosToPixel(pos);
                    const auto idispl = displ.round();
                    mGamePainter.translate(idispl.fX, idispl.fY);
                    auto& model = u->model();
                    model.incFrame(1.f);
                    bool highlight = false;
                    if(!mHighlightUnit && u != mMainChar && u->fHealth > 0) {
                        const SDL_Point p{int(mousePos.fX), int(mousePos.fY)};
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
                    if(mPressedUnit) highlight = mPressedUnit == u;
                    model.draw(mGamePainter, highlight);
                    mGamePainter.restore();
                } else if(e.fType == eRenderElementType::missile) {
                    const auto m = std::static_pointer_cast<eExtendedMissile>(e.fPtr);
                    if(!m) continue;
                    const auto& pos = m->fPos;
                    const auto iPos = pos.floor();
                    if(iPos.fY != y) continue;
                    if(iPos.fX != x) continue;
                    const auto pixel = tilePosToPixel(m->fPos);
                    auto& fireball = eMissilesTextures::sMissiles.get(m->fType);
                    const int dirs = fireball.nDirs(0);
                    const float ainc = 360.f/dirs;
                    int dir = std::round(m->fAngle/ainc) + 2*dirs/16;
                    dir = (dirs + dir) % dirs;
                    const float lradius = fireball.lighting();
                    if(lradius > 0.01f) {
                        mGamePainter.renderLight(r, pixel.fX, pixel.fY,
                                                 lradius, SDL_Color{255, 255, 255, 255});
                    }
                    const auto& ftex = fireball.get(0, dir, mFrame % fireball.nFrames(0));
                    mGamePainter.drawTexture(pixel.fX, pixel.fY, ftex);
                } else if(e.fType == eRenderElementType::item) {
                    const auto i = std::static_pointer_cast<eGroundItem>(e.fPtr);
                    if(!i) continue;
                    const auto& pos = i->fPos;
                    const auto iPos = pos.floor();
                    if(iPos.fY != y) continue;
                    if(iPos.fX != x) continue;
                    const auto pixel = tilePosToPixel(i->fPos);
                    mGamePainter.fillRect(SDL_Rect{int(pixel.fX) - 2, int(pixel.fY) - 2,
                                                   4, 4}, SDL_Color{255, 0, 0, 255});
                }
                nextElement = eleId + 1;
            }
        });
    }

    mGamePainter.renderLight(r, mInput.characterHorizontalPos()*width(),
                             mInput.characterVerticalPos()*height(),
                             10.f, SDL_Color{255, 255, 255, 255});
    mGamePainter.finish();

    const auto& tex = texture();
    if(tex) {
        p.drawTexture(rect(), tex, eAlignment::center);
    }
}

bool eGameWidget::mousePressEvent(const eMouseEvent& e) {
    const auto button = e.button();
    const bool leftPressed = static_cast<bool>(
        button & eMouseButton::left);
    const bool rightPressed = static_cast<bool>(
        button & eMouseButton::right);
    if(leftPressed || rightPressed) {
        mInput.handleMousePress(leftPressed, rightPressed,
                                float(e.x()), float(e.y()));
        if(mHighlightUnit) {
            setPressedUnit(mHighlightUnit);
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
        const int skillId = leftReleased ? mLeftSkill : mRightSkill;
        const auto& skill = eSkills::sSkills.get(skillId);
        const auto& eq = mMainAction.weaponData();
        const bool rangeAttack = eMainCharAction::sRangedAttack(
            skillId, skill.fType, eq);
        if(e.shiftPressed() || (rightRelease && rangeAttack) ||
           (rangeAttack && mPressedUnit)) {
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
    if(mUnitIndicator && !mPressedUnit) {
        mUnitIndicator->setUnit(u);
    }
}

void eGameWidget::setPressedUnit(const std::shared_ptr<eUnit>& u) {
    mPressedUnit = u;
    if(mUnitIndicator) {
        if(mPressedUnit) {
            mUnitIndicator->setUnit(mPressedUnit);
        } else {
            mUnitIndicator->setUnit(mHighlightUnit);
        }
    }

    mMainAction.setPressedUnit(u);
}

eWalkable eGameWidget::walkable() const {
    return [this](const int x, const int y) {
        return mMap->walkable(x, y);
    };
}

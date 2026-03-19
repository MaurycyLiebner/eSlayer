#include "egamescreen.h"

#include "../elanguage.h"
#include "../emainwindow.h"
#include "../textures/echarstextures.h"
#include "../textures/eobjstextures.h"
#include "../textures/eterrstextures.h"
#include "../textures/etilesiterator.h"
#include "../textures/euitextures.h"
#include "../textures/emissilestextures.h"
#include "../widgets/echeckbutton.h"
#include "../widgets/ecolors.h"
#include "../widgets/gameScreen/eescmenubutton.h"
#include "../widgets/gameScreen/eplayerhealthindicator.h"
#include "../widgets/gameScreen/eunitindicator.h"

#include <eSlayerMissiles/emissileincrement.h>

#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eunittile.h>
#include <eSlayerHelpers/evec2.h>

eGameScreen::eGameScreen(eMainWindow* const window) :
    eScreenBase(window),
    mGamePainter(renderer()) {}

eGameScreen::~eGameScreen() {
    if(mServer) {
        mServer->disconnect(mClientId);
    }
}

void eGameScreen::setExitAction(const eAction& a) {
    mExitAction = a;
}

void eGameScreen::initialize(const int clientId,
                             const std::shared_ptr<eServer>& server,
                             const std::shared_ptr<eMap>& map) {
    mClientId = clientId;
    mServer = server;

    initializeTextures();
    mUnitIndicator = new eUnitIndicator(window());
    mUnitIndicator->initialize();
    const float m = resolution().multiplier();
    mUnitIndicator->resize(200*m, 40*m);
    addWidget(mUnitIndicator);
    mUnitIndicator->align(eAlignment::hcenter | eAlignment::top);
    mUnitIndicator->setY(20*m);

    const int indicatorW = 400*m;
    const int indicatorH = 30*m;

    const auto bottomWid = new eWidget(window());
    bottomWid->setNoPadding();

    const auto attackIcon = eUITextures::sSkillIcons["attack"];
    mLeftSkillButton = new eButtonBase(window());
    mLeftSkillButton->setNoPadding();
    mLeftSkillButton->setTexture(attackIcon);
    mLeftSkillButton->fitContent();
    bottomWid->addWidget(mLeftSkillButton);

    const auto centerWid = new eWidget(window());
    centerWid->setNoPadding();

    mExperienceIndicator = new ePlayerHealthIndicator(window());
    mExperienceIndicator->setColor(eColors::sExperience);
    mExperienceIndicator->setName(eLanguage::text(7, 3));
    mExperienceIndicator->initialize();
    centerWid->addWidget(mExperienceIndicator);
    mExperienceIndicator->resize(2*indicatorW, indicatorH/2);

    const auto healthMana = new eWidget(window());
    healthMana->setNoPadding();

    mHealthIndicator = new ePlayerHealthIndicator(window());
    mHealthIndicator->setColor(eColors::sHealth);
    mHealthIndicator->setName(eLanguage::text(7, 0));
    mHealthIndicator->initialize();
    healthMana->addWidget(mHealthIndicator);
    mHealthIndicator->resize(indicatorW, indicatorH);

    mManaIndicator = new ePlayerHealthIndicator(window());
    mManaIndicator->setColor(eColors::sMana);
    mManaIndicator->setName(eLanguage::text(7, 1));
    mManaIndicator->initialize();
    healthMana->addWidget(mManaIndicator);
    mManaIndicator->resize(indicatorW, indicatorH);

    const int lineWidth = eLabel::lineWidth();
    healthMana->stackHorizontally(-lineWidth);
    healthMana->fitContent();
    centerWid->addWidget(healthMana);

    const auto staminaExperience = new eWidget(window());
    staminaExperience->setNoPadding();

    const auto staminaWid = new eWidget(window());
    staminaWid->setNoPadding();

    mRunButton = new eCheckButton(window());
    mRunButton->setNoPadding();
    mRunButton->setCheckAction([this](const bool check) {
        if(check) mRunButton->setTexture(eUITextures::sRunIcon);
        else mRunButton->setTexture(eUITextures::sWalkIcon);
        mMainAction.setRunning(check);
    });
    mRunButton->setTexture(eUITextures::sWalkIcon);
    mRunButton->fitContent();
    staminaWid->addWidget(mRunButton);

    mStaminaIndicator = new ePlayerHealthIndicator(window());
    mStaminaIndicator->setColor(eColors::sStamina);
    mStaminaIndicator->setName(eLanguage::text(7, 2));
    mStaminaIndicator->initialize();
    staminaWid->addWidget(mStaminaIndicator);
    mStaminaIndicator->resize(indicatorW - mRunButton->width(),
                              mRunButton->height());

    staminaWid->stackHorizontally();
    staminaWid->fitContent();

    staminaExperience->addWidget(staminaWid);

    staminaExperience->stackHorizontally(-lineWidth);
    staminaExperience->fitContent();
    centerWid->addWidget(staminaExperience);

    centerWid->stackVertically();
    centerWid->fitContent();
    centerWid->align(eAlignment::bottom | eAlignment::hcenter);
    bottomWid->addWidget(centerWid);

    mRightSkillButton = new eButtonBase(window());
    mRightSkillButton->setNoPadding();
    mRightSkillButton->setTexture(attackIcon);
    mRightSkillButton->fitContent();
    bottomWid->addWidget(mRightSkillButton);

    bottomWid->stackHorizontally();
    bottomWid->fitContent();
    addWidget(bottomWid);
    bottomWid->align(eAlignment::bottom | eAlignment::hcenter);

    mMap = map;

    const auto r = renderer();

    const auto w = walkable();
    const auto iter = [this](const eOtherHandler& handler) {
        for(const auto& u : mUnits) {
            if(!u) continue;
            handler(*u);
        }
    };
    mMainAction.initialize(mServer, r, w, iter, clientId, 0);
    mMainChar = mMainAction.unit();
    // {
    //     const auto dir = "/home/ailuropoda/.eSlayer/tmp/preview/";
    //     for(const auto& entry : std::filesystem::directory_iterator(dir))
    //         std::filesystem::remove_all(entry.path());
    //     const eCharTextures::eModelParts modelParts {
    //         {"mummy", "whole"}
    //     };
    //     const auto texs = eCharsTextures::get("mummy");
    //     const auto unitModel = texs->generateModel(modelParts, r);
    //     eCharUnitModel model;
    //     model.setCharModel(unitModel);
    //     model.setDirection(0);
    //     model.generatePreview(r);
    // }
}

const ePointF& eGameScreen::characterPos() const {
    return mMainAction.pos();
}

ePointF eGameScreen::pixelToTilePos(
    const ePointF& pos,
    const ePointF& pixel) const {
    ePointF result;
    result.fY = pos.fY +
                (pixel.fY - height()/2.f)/mTileH +
                (width()/2.f - pixel.fX)/mTileW;
    result.fX = pos.fX +
                (pixel.fX - width()/2.f)/mTileW +
                (pixel.fY - height()/2.f)/mTileH;
    return result;
}

ePointF eGameScreen::pixelToTilePos(
    const ePointF& pixel) const {
    const auto& pos = characterPos();
    return pixelToTilePos(pos, pixel);
}

ePointF eGameScreen::tilePosToPixel(const ePointF& pos) const {
    const auto& charPos = characterPos();
    ePointF result;
    result.fY = height()/2.f + (pos.fY - charPos.fY + pos.fX - charPos.fX)*mTileH/2.f;
    result.fX = width()/2.f + (charPos.fY - pos.fY + pos.fX - charPos.fX)*mTileW/2.f;
    return result;
}

void eGameScreen::paintEvent(ePainter& p) {
    mGamePainter.clear();

    const float by = 1.f;
    mServer->increment(by);
    mServer->requestData(mClientId);
    eRequestData data;
    float resultTime;
    const auto r = renderer();
    const bool b = mServer->receiveData(
        mClientId, data, resultTime);
    if(b) {
        mUnitAreas.clear();
        const auto& units = data.fUnits;
        const auto& missiles = data.fMissiles;
        bool aggressive = false;
        std::set<int> present;
        for(const auto& u : units) {
            const int charId = u.fCharId;
            const auto ipos = u.fPos.floor();
            eUnitTile tile;
            reinterpret_cast<ePoint&>(tile) = ipos;
            mUnitAreas[tile].emplace(charId);
            present.emplace(charId);
            if(charId == mClientId) {
                if(mMainChar->fHealth <= 0 && u.fHealth > 0) {
                    if(mDeadMenu) {
                        mDeadMenu->deleteLater();
                        mDeadMenu = nullptr;
                    }
                    mMainChar->fPos = u.fPos;
                    mMainAction.setPos(u.fPos);
                    mMainAction.stop();
                    setPressedUnit(nullptr);
                    setHighlightedUnit(nullptr);
                }
                mMainChar->fHealth = u.fHealth;
                if(!mDeadMenu && u.fHealth <= 0) {
                    showDeadMenu();
                    mMainAction.stop();
                }
                mMainChar->fMaxHealth = u.fMaxHealth;
                mMainChar->fActionTime = u.fActionTime;
                if(u.fAnimId > mMainChar->fAnimId) {
                    mMainChar->fAnim = u.fAnim;
                    mMainChar->fAnimId = u.fAnimId;
                    mMainChar->fAnimSpeed = u.fAnimSpeed;
                }
                mHealthIndicator->setValue(u.fHealth);
                mHealthIndicator->setRange(0, u.fMaxHealth);
                mStaminaIndicator->setValue(mMainAction.stamina());
                mStaminaIndicator->setRange(0, mMainAction.maxStamina());
                continue;
            }
            const auto unit = mUnits.get(charId);
            if(unit) {
                reinterpret_cast<eUnitData&>(*unit) = u;
                auto& model = unit->model();
                unit->fPos = u.fPos;
                model.setAngle(u.fAngle);
                model.setAnimation(unit->fAnim, unit->fAnimId, u.fAnimSpeed);
            } else {
                const auto texs = eCharsTextures::get(u.fTypeId);
                const auto unitModel = texs->generateModel(u.fModelParts, r);

                const auto unit = std::make_shared<eUnit>();
                unit->fRadius = u.fRadius;
                reinterpret_cast<eUnitData&>(*unit) = u;
                eCharUnitModel model;
                model.setCharModel(unitModel);
                model.setAnimation(u.fAnim, u.fAnimId, u.fAnimSpeed);
                model.setAngle(u.fAngle);
                unit->setModel(model);
                unit->fPos = u.fPos;
                mUnits.add(charId, unit);
            }
            if(!aggressive && mMainChar->fTeamId != u.fTeamId && u.fHealth > 0) {
                const float dist = ePointF::distance(mMainChar->fPos, u.fPos);
                if(dist < 5.f) aggressive = true;
            }
        }
        for(const auto& u : mUnits) {
            const int charId = u->fCharId;
            const auto it = present.find(charId);
            if(it != present.end()) continue;
            mUnits.remove(charId);
        }
        auto& model = mMainChar->model();
        model.setAggressive(aggressive);

        for(const auto& m : missiles) {
            const auto mm = std::make_shared<eExtendedMissile>();
            reinterpret_cast<eMissile&>(*mm) = m;
            mMissiles.add(m.fId, mm);
        }
    }

    mServer->changeState(mClientId, *mMainChar);
    for(const auto& m : mMissiles) {
        const auto oldPos = m->fPos;
        eMissileIncrement::increment(*m, by);
        const auto newPos = m->fPos;
        const auto dir = ePointF::vector(oldPos, newPos);
        m->fAngle = dir.angle();
        if(m->fRemDist <= 0.0001f) {
            mMissiles.remove(m->fId);
        } else {
            const auto ipos = m->fPos.floor();
            const bool obsticle = !mMap->walkable(ipos.fX, ipos.fY);
            if(obsticle) {
                mMissiles.remove(m->fId);
            } else {
                bool found = false;
                const int margin = int(m->fRadius) + 1;
                const int xMin = ipos.fX - margin;
                const int xMax = ipos.fX + margin;
                const int yMin = ipos.fY - margin;
                const int yMax = ipos.fY + margin;
                for(int x = xMin; x <= xMax; x++) {
                    for(int y = yMin; y <= yMax; y++) {
                        const auto& charIds = mUnitAreas[eUnitTile{x, y}];
                        for(const int charId : charIds) {
                            const auto& u = mUnits.get(charId);
                            if(!u) continue;
                            if(u->fTeamId == m->fTeamId) continue;
                            const float dist = ePointF::distance(u->fPos, m->fPos);
                            if(dist > 0.5f*(u->fRadius + m->fRadius)) continue;
                            found = true;
                            mMissiles.remove(m->fId);
                            break;
                        }
                    }
                    if(found) break;
                }
            }
        }
    }

    if(!mESCMenu && !mDeadMenu) {
        const auto mouseTilePos = pixelToTilePos(mMousePos);
        const auto w = window();
        const bool shiftPressed = w->shiftPressed();
        mMainAction.increment(mMousePressed, shiftPressed,
                              mouseTilePos, by);
    }

    mFrame++;

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
                mGamePainter.drawTexture(px, py, tex, eAlignment::top | eAlignment::hcenter);
                const auto iobjs = mMap->objects(x, y);
            });
        }

        enum class eRenderElementType {
            unit, missile
        };

        struct eRenderElement {
            eRenderElementType fType;
            std::shared_ptr<ePositioned> fPtr;
        };
        std::vector<eRenderElement> renderElements;
        const int margin = 100;
        const int w = width();
        const int h = height();
        for(const auto& u : mUnits) {
            const auto pixel = tilePosToPixel(u->fPos);
            if(pixel.fX < -margin || pixel.fY < -margin ||
               pixel.fX > w + margin || pixel.fY > h + margin) continue;
            renderElements.emplace_back(eRenderElement{eRenderElementType::unit,
                                        std::static_pointer_cast<ePositioned>(u)});
        }
        renderElements.emplace_back(eRenderElement{eRenderElementType::unit,
                                    std::static_pointer_cast<ePositioned>(mMainChar)});
        for(const auto& m : mMissiles) {
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
        iterator.iterate([&](const int x, const int y,
                             const int px, const int py) {
            const auto& iobjs = mMap->objects(x, y);
            for(const auto& iobj : iobjs) {
                const auto& obj = mMap->object(iobj);
                const auto& objType = objTypes[obj.fObjectType];
                const auto object = eObjsTextures::get(objType.fName);
                const auto& tex = object->getTexture(obj.fTileType);
                p.drawTexture(px, py, tex, eAlignment::top | eAlignment::hcenter);
            }
            for(int eleId = nextElement; eleId < renderElements.size(); eleId++) {
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
                        const SDL_Point p{int(mMousePos.fX), int(mMousePos.fY)};
                        const int w = 0.75*u->fRadius*mTileW;
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
                }
                nextElement = eleId + 1;
            }
        });
    }

    mGamePainter.renderLight(r, width()/2.f, height()/2.f,
                             10.f, SDL_Color{255, 255, 255, 255});
    mGamePainter.finish();

    eLabel::paintEvent(p);
}

bool eGameScreen::mousePressEvent(const eMouseEvent& e) {
    const auto button = e.button();
    const bool leftPressed = static_cast<bool>(
        button & eMouseButton::left);
    if(leftPressed) {
        mMousePressed = true;
        mMousePos = ePointF{float(e.x()), float(e.y())};
        if(mHighlightUnit) {
            setPressedUnit(mHighlightUnit);
        }
    }
    return true;
}

bool eGameScreen::mouseReleaseEvent(const eMouseEvent& e) {
    const auto button = e.button();
    const bool leftReleased = static_cast<bool>(
        button & eMouseButton::left);
    if(leftReleased) {
        mMousePressed = false;
        setPressedUnit(nullptr);
        if(e.shiftPressed()) {
            mMainAction.stop();
        } else {
            const auto pos = pixelToTilePos(mMousePos);
            mMainAction.mouseRelease(pos);
        }
    }
    return true;
}

bool eGameScreen::mouseMoveEvent(const eMouseEvent& e) {
    mMousePos = ePointF{float(e.x()), float(e.y())};
    return true;
}

bool eGameScreen::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_ESCAPE) {
        if(mDeadMenu) {
            mServer->respawn(mClientId);
        } else {
            if(mESCMenu) {
                hideESCMenu();
            } else {
                showESCMenu();
            }
        }
    } else if(e.key() == SDL_SCANCODE_R) {
        const bool run = !mMainAction.running();
        mRunButton->setChecked(run);
        mMainAction.setRunning(run);
    }
    return true;
}

void eGameScreen::initializeTextures() {
    const int w = width();
    const int h = height();
    const auto tex = mGamePainter.initialize(w, h);
    setTexture(tex);
}

void eGameScreen::showDeadMenu() {
    mDeadMenu = new eWidget(window());

    const auto line1 = new eLabel(window());
    line1->setExtraHugeFontSize();
    line1->setFontColor(eFontColor::redBlack);
    line1->setText(eLanguage::text(5, 3));
    line1->fitContent();
    mDeadMenu->addWidget(line1);

    const auto line2 = new eLabel(window());
    line2->setExtraHugeFontSize();
    line2->setFontColor(eFontColor::redBlack);
    line2->setText(eLanguage::text(5, 4));
    line2->fitContent();
    mDeadMenu->addWidget(line2);

    const auto res = resolution();
    const int p = res.hugePadding();
    mDeadMenu->stackVertically(p);
    mDeadMenu->fitContent();

    line1->align(eAlignment::hcenter);
    line2->align(eAlignment::hcenter);

    addWidget(mDeadMenu);
    mDeadMenu->align(eAlignment::center);
}

void eGameScreen::showESCMenu() {
    mESCMenu = new eWidget(window());

    const auto optionsB = new eESCMenuButton(
        eLanguage::text(5, 0), window());
    mESCMenu->addWidget(optionsB);

    const auto exitB = new eESCMenuButton(
        eLanguage::text(5, 1), window());
    mESCMenu->addWidget(exitB);
    exitB->setPressAction([this]() {
        if(mServer) {
            mServer->disconnect(mClientId);
            mServer = nullptr;
        }
        mExitAction();
    });

    const auto returnB = new eESCMenuButton(
        eLanguage::text(5, 2), window());
    mESCMenu->addWidget(returnB);
    returnB->setPressAction([this]() {
        hideESCMenu();
    });

    const auto res = resolution();
    const int p = res.hugePadding();
    mESCMenu->stackVertically(p);
    mESCMenu->fitContent();

    optionsB->align(eAlignment::hcenter);
    exitB->align(eAlignment::hcenter);
    returnB->align(eAlignment::hcenter);

    addWidget(mESCMenu);
    mESCMenu->align(eAlignment::center);
}

void eGameScreen::hideESCMenu() {
    if(!mESCMenu) return;
    mESCMenu->deleteLater();
    mESCMenu = nullptr;
}

void eGameScreen::setHighlightedUnit(const std::shared_ptr<eUnit>& u) {
    mHighlightUnit = u;
    if(!mPressedUnit) {
        mUnitIndicator->setUnit(u);
    }
}

void eGameScreen::setPressedUnit(const std::shared_ptr<eUnit>& u) {
    mPressedUnit = u;
    if(mPressedUnit) {
        mUnitIndicator->setUnit(mPressedUnit);
    } else {
        mUnitIndicator->setUnit(mHighlightUnit);
    }

    mMainAction.setPressedUnit(u);
}

eWalkable eGameScreen::walkable() const {
    return [this](const int x, const int y) {
        return mMap->walkable(x, y);
    };
}

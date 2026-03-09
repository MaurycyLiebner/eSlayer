#include "egamescreen.h"

#include "../elanguage.h"
#include "../textures/echarstextures.h"
#include "../textures/eobjstextures.h"
#include "../textures/eterrstextures.h"
#include "../textures/etilesiterator.h"
#include "../widgets/gameScreen/eescmenubutton.h"
#include "../widgets/gameScreen/eunitindicator.h"

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
    const double m = resolution().multiplier();
    mUnitIndicator->resize(200*m, 40*m);
    addWidget(mUnitIndicator);
    mUnitIndicator->align(eAlignment::hcenter | eAlignment::top);
    mUnitIndicator->move(mUnitIndicator->x(), 20*m);

    mMap = map;

    const eCharTextures::eModelParts modelParts {
        {"whole", "light"}
    };
    mMainCharData = eCharsTextures::get("pal");

    const auto r = renderer();
    const auto model = mMainCharData->generateModel(modelParts, r);
    eCharUnitModel umodel;
    umodel.setCharModel(model);
    umodel.setAnimation(0);
    umodel.setDirection(0);

    const auto w = walkable();
    const auto iter = [this](const eOtherHandler& handler) {
        for(const auto& u : mUnits) {
            handler(*u);
        }
    };
    mMovementHandler.intialize(w, iter, clientId, 0);
    mMovementHandler.setRadius(0.4);
    mMovementHandler.setMoveRandom(0.);

    mMainChar = std::make_shared<eUnit>();
    mMainChar->setModel(umodel);

    // {
    //     const auto dir = "/home/ailuropoda/.eSlayer/tmp/preview/";
    //     for(const auto& entry : std::filesystem::directory_iterator(dir))
    //         std::filesystem::remove_all(entry.path());
    //     eCharModel unitModel;
    //     const eCharTextures::eModelParts modelParts {
    //         {"wendigo", "whole"}
    //     };
    //     const auto texs = eCharsTextures::get("wendigo");
    //     unitModel = texs->generateModel(modelParts, r);
    //     eCharUnitModel model;
    //     model.setCharModel(unitModel);
    //     model.setDirection(0);
    //     model.generatePreview(r);
    // }
}

const ePointF& eGameScreen::characterPos() const {
    return mMovementHandler.pos();
}

ePointF eGameScreen::pixelToTilePos(
    const ePointF& pos,
    const ePointF& pixel) const {
    ePointF result;
    result.fY = pos.fY +
                (pixel.fY - height()/2.)/mTileH +
                (width()/2. - pixel.fX)/mTileW;
    result.fX = pos.fX +
                (pixel.fX - width()/2.)/mTileW +
                (pixel.fY - height()/2.)/mTileH;
    return result;
}

ePointF eGameScreen::pixelToTilePos(
    const ePointF& pixel) const {
    const auto& pos = mMovementHandler.pos();
    return pixelToTilePos(pos, pixel);
}

ePointF eGameScreen::tilePosToPixel(const ePointF& pos) const {
    const auto& charPos = mMovementHandler.pos();
    ePointF result;
    result.fY = height()/2. + (pos.fY - charPos.fY + pos.fX - charPos.fX)*mTileH/2.;
    result.fX = width()/2. + (charPos.fY - pos.fY + pos.fX - charPos.fX)*mTileW/2.;
    return result;
}

void eGameScreen::paintEvent(ePainter& p) {
    mGamePainter.clear();

    mServer->increment(1.);
    mServer->requestUnits(mClientId);
    std::vector<eUnitData> units;
    double resultTime;
    const auto r = renderer();
    const bool b = mServer->receiveUnits(
        mClientId, units, resultTime);
    if(b) {
        for(const auto& u : units) {
            const int charId = u.fCharId;
            if(charId == mClientId) continue;
            const auto it = mUnitIndexMap.find(charId);
            if(it == mUnitIndexMap.end()) {
                std::shared_ptr<eCharModel> unitModel;
                if(u.fTypeId == 0) {
                    const eCharTextures::eModelParts modelParts {
                        {"mummy", "whole"}
                    };
                    const auto texs = eCharsTextures::get("mummy");
                    unitModel = texs->generateModel(modelParts, r);
                } else {
                    const eCharTextures::eModelParts modelParts {
                        {"wendigo", "whole"}
                    };
                    const auto texs = eCharsTextures::get("wendigo");
                    unitModel = texs->generateModel(modelParts, r);
                }
                auto& unit = mUnits.emplace_back(std::make_shared<eUnit>());
                unit->fRadius = u.fRadius;
                reinterpret_cast<eUnitData&>(*unit) = u;
                initialize(charId, *unit);
                eCharUnitModel model;
                model.setCharModel(unitModel);
                model.setAnimation(0);
                model.setDirection(0);
                unit->setModel(model);
                unit->setPos(u.fPos);
                mUnitIndexMap[charId] = mUnits.size() - 1;
            } else {
                const int id = it->second;
                const auto& unit = mUnits[id];
                reinterpret_cast<eUnitData&>(*unit) = u;
                auto& model = unit->model();
                unit->setPos(u.fPos);
                model.setAngle(u.fAngle);
                model.setAnimation(unit->fAnim, unit->fAnimId);
            }
        }
    }
    mServer->moveTo(mClientId, characterPos());

    if(!mESCMenu) {
        const auto pos = pixelToTilePos(mMousePos);
        bool move = false;
        eVec2d vec;

        if(mMousePressed) {
            mMovementHandler.moveInDirection(pos);
            move = mMovementHandler.increment(1.);
        }
        if(!move) {
            if(mMousePressed) mMovementHandler.moveTo(pos);
            move = mMovementHandler.increment(1.);
        }
        auto& model = mMainChar->model();
        const bool a = false;
        int animId;
        if(move) {
            model.setAngle(mMovementHandler.angle());
            const int naId = mMainCharData->animId("walk");
            const int aId = mMainCharData->animId("walkReady");
            if(a) {
                if(aId != -1) {
                    animId = aId;
                } else {
                    animId = naId;
                }
            } else {
                if(naId != -1) {
                    animId = naId;
                } else {
                    animId = aId;
                }
            }
        } else {
            const int naId = mMainCharData->animId("stand");
            const int aId = mMainCharData->animId("standReady");
            if(a) {
                if(aId != -1) {
                    animId = aId;
                } else {
                    animId = naId;
                }
            } else {
                if(naId != -1) {
                    animId = naId;
                } else {
                    animId = aId;
                }
            }
        }
        model.setAnimation(animId);
        mFrame++;
    }

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

        const auto pos = characterPos();
        std::vector<std::shared_ptr<eUnit>> units = mUnits;
        units.emplace_back(mMainChar);
        mMainChar->setPos(pos);

        std::sort(units.begin(), units.end(), [&](const std::shared_ptr<eUnit>& u1,
                                                  const std::shared_ptr<eUnit>& u2) {
            const auto& p1 = u1->pos();
            const auto& p2 = u2->pos();
            const auto& ip1 = p1.floor();
            const auto& ip2 = p2.floor();

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

        int nextUnit = 0;
        setHighlightedUnit(nullptr);
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
            for(int unitId = nextUnit; unitId < units.size(); unitId++) {
                const auto& u = units[unitId];
                const auto& pos = u->pos();
                const auto iPos = pos.floor();
                if(iPos.fY != y) continue;
                if(iPos.fX != x) continue;
                mGamePainter.save();
                const auto displ = tilePosToPixel(pos);
                const auto idispl = displ.round();
                mGamePainter.translate(idispl.fX, idispl.fY);
                const int frame = std::round(resultTime - u->actionStartTime());
                auto& model = u->model();
                model.incFrame(1.);
                bool highlight = false;
                if(!mHighlightUnit && u != mMainChar) {
                    const SDL_Point p{int(mMousePos.fX), int(mMousePos.fY)};
                    const int w = 0.75*u->fRadius*mTileW;
                    const int h = 2.*w;
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
                model.draw(mGamePainter, highlight);
                mGamePainter.restore();
                nextUnit = unitId + 1;
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
        mMousePos = ePointF{double(e.x()), double(e.y())};
    }
    return true;
}

bool eGameScreen::mouseReleaseEvent(const eMouseEvent& e) {
    const auto button = e.button();
    const bool leftReleased = static_cast<bool>(
        button & eMouseButton::left);
    if(leftReleased) {
        mMousePressed = false;
        const auto pos = pixelToTilePos(mMousePos);
        mMovementHandler.moveTo(pos);
    }
    return true;
}

bool eGameScreen::mouseMoveEvent(const eMouseEvent& e) {
    mMousePos = ePointF{double(e.x()), double(e.y())};
    return true;
}

bool eGameScreen::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_ESCAPE) {
        if(mESCMenu) {
            hideESCMenu();
        } else {
            showESCMenu();
        }
    }
    return true;
}

void eGameScreen::initializeTextures() {
    const int w = width();
    const int h = height();
    const auto tex = mGamePainter.initialize(w, h);
    setTexture(tex);
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
    mUnitIndicator->setUnit(u);
}

eWalkable eGameScreen::walkable() const {
    return [this](const int x, const int y) {
        return mMap->walkable(x, y);
    };
}

void eGameScreen::initialize(const int charId, eUnit& u) {
    const auto w = walkable();
    u.intialize(w, charId);
}

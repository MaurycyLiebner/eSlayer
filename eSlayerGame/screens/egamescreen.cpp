#include "egamescreen.h"

#include "../textures/echarstextures.h"
#include "../textures/eterrstextures.h"
#include "../textures/eobjstextures.h"
#include "../textures/etilesiterator.h"

#include "../widgets/gameScreen/eescmenubutton.h"
#include "../elanguage.h"

#include <eSlayerHelpers/epathsmoother.h>
#include <eSlayerHelpers/evec2.h>

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

    mMap = map;

    const eCharTextures::eModelParts modelParts {
        {"whole", "light"}
    };
    const auto texs = eCharsTextures::get("pal");

    const auto r = renderer();
    const auto model = texs->generateModel(modelParts, r);
    mModel.setCharModel(model);
    mModel.setAnimation(0);
    mModel.setDirection(0);

    const auto w = walkable();
    const auto o = obsticle();
    mMovementHandler.intialize(w, o, clientId);
    mMovementHandler.setDivergeAngle(90.);
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
    mServer->increment();
    mServer->requestUnits(mClientId);
    std::vector<std::shared_ptr<eServerUnit>> units;
    const int delay = mServer->receiveUnits(mClientId, units);
    if(delay != -1) {
        for(const auto& u : units) {
            const int charId = u->fCharId;
            if(charId == mClientId) continue;
            const auto it = mUnitIndexMap.find(charId);
            if(it == mUnitIndexMap.end()) {
                const eCharTextures::eModelParts modelParts {
                    {"mummy", "whole"}
                };
                const auto r = renderer();
                const auto texs = eCharsTextures::get("mummy");
                const auto unitModel = texs->generateModel(modelParts, r);
                auto& unit = mUnits.emplace_back(std::make_shared<eUnit>());
                initialize(charId, *unit);
                eCharUnitModel model;
                model.setCharModel(unitModel);
                model.setAnimation(0);
                model.setDirection(0);
                unit->setModel(model);
                unit->setPos(u->fPos);
                mUnitIndexMap[charId] = mUnits.size() - 1;
            } else {
                const int id = it->second;
                const auto& uu = mUnits[id];
                auto& model = uu->model();
                const auto& planned = u->fPlanned;
                uu->pushPlanned(planned);
            }
        }
    }
    mServer->moveTo(mClientId, characterPos());

    if(!mESCMenu) {
        bool move = false;
        eVec2d vec;
        const auto pos = pixelToTilePos(mMousePos);

        if(mMousePressed) {
            mMovementHandler.moveInDirection(pos);
            move = mMovementHandler.increment(1.);
        }
        if(!move) {
            if(mMousePressed) mMovementHandler.moveTo(pos);
            move = mMovementHandler.increment(1.);
        }
        if(move) {
            mModel.setAngle(mMovementHandler.angle());
            mModel.setAnimation(1);
        } else {
            mModel.setAnimation(0);
        }
        mFrame++;
    }

    const auto r = renderer();
    {
        const auto holder = mBaseTex->createTargetHolder(r);

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
                p.drawTexture(px, py, tex, eAlignment::top | eAlignment::hcenter);
                const auto iobjs = mMap->objects(x, y);
            });
        }

        const auto pos = characterPos();
        std::vector<std::shared_ptr<eUnit>> units = mUnits;
        auto& mainChar = units.emplace_back(std::make_shared<eUnit>());
        mainChar->setPos(pos);
        mainChar->setModel(mModel);

        for(const auto& u : mUnits) {
            u->increment(1.);
        }

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
                p.save();
                const auto displ = tilePosToPixel(pos);
                const auto idispl = displ.round();
                p.translate(idispl.fX, idispl.fY);
                u->model().draw(p, mFrame);
                p.restore();
                nextUnit = unitId + 1;
            }
        });
    }

    {
        const auto holder = mLightingTex->createTargetHolder(r);
        mLightingTex->clear(r);
        mLightingTex->renderLight(r, width()/2.f, height()/2.f,
                                  10.f, SDL_Color{255, 255, 255, 255});
    }

    {
        const auto holder = mDisplayTex->createTargetHolder(r);
        mBaseTex->setBlendMode(SDL_BLENDMODE_BLEND);
        mBaseTex->render(r, 0, 0);
        mLightingTex->render(r, 0, 0);
        mBaseTex->fill(r, SDL_Color{255, 255, 255, 115});
        mBaseTex->setBlendMode(SDL_BLENDMODE_MUL);
        mBaseTex->render(r, 0, 0);
    }

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
    const auto r = renderer();

    mBaseTex = std::make_shared<eTexture>();
    mBaseTex->create(r, w, h, {0, 0, 0, 255});

    mLightingTex = std::make_shared<eLightingTexture>();
    mLightingTex->initialize(r, w, h, SDL_Color{180, 180, 180, 255});

    mDisplayTex = std::make_shared<eTexture>();
    mDisplayTex->create(r, w, h, {0, 0, 0, 255});

    setTexture(mDisplayTex);
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

eWalkable eGameScreen::walkable() const {
    return [this](const int x, const int y) {
        return mMap->walkable(x, y);
    };
}

eObsticle eGameScreen::obsticle() const {
    return [this](const int charId, const ePointF& p) {
        for(const auto& u : mUnits) {
            if(u->charId() == charId) continue;
            const auto& pos = u->pos();
            const auto dist = ePointF::distance(pos, p);
            if(dist < 0.4) return true;
        }
        if(charId != mClientId) {
            const auto& pos = mMovementHandler.pos();
            const auto dist = ePointF::distance(pos, p);
            if(dist < 0.4) return true;
        }
        return false;
    };
}

void eGameScreen::initialize(const int charId, eUnit& u) {
    const auto w = walkable();
    const auto o = obsticle();
    u.intialize(w, o, charId);
}

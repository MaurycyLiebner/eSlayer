#include "egamescreen.h"

#include "../textures/echarstextures.h"
#include "../textures/eterrstextures.h"
#include "../textures/eobjstextures.h"

#include "../widgets/gameScreen/eescmenubutton.h"
#include "../elanguage.h"

#include <eSlayerHelpers/epathsmoother.h>
#include <eSlayerHelpers/evec2.h>

void eGameScreen::setExitAction(const eAction& a) {
    mExitAction = a;
}

void eGameScreen::initialize(const std::shared_ptr<eMap>& map) {
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
}

ePointF eGameScreen::pixelToTilePos(
    const ePointF& pos,
    const ePointF& pixel) const {
    ePointF result;
    result.fY = pos.fY +
                (pixel.fY - height()/2.f)/mTileH +
                (width()/2.f - pixel.fX)/mTileW;
    result.fX = pos.fX +
                (pixel.fX - width()/2.)/mTileW +
                (pixel.fY - height()/2.)/mTileH;
    return result;
}

ePointF eGameScreen::pixelToTilePos(
    const ePointF& pixel) const {
    return pixelToTilePos(mPos, pixel);
}

void eGameScreen::updateTargetPos() {
    const auto pos = pixelToTilePos(mMousePos);
    setTargetPos(pos);
}

void eGameScreen::setTargetPos(const ePointF& pos) {
    const int margin = mPathFindMargin;
    const double subdivision = mTileMoveSubdivision;
    const int dim = 2*margin + 1;
    ePathFinderMap map(dim, dim);
    const auto iPos = (mPos * subdivision).round();
    for(int sx = 0; sx < dim; sx++) {
        for(int sy = 0; sy < dim; sy++) {
            const int x = (iPos.fX + sx - margin)/mTileMoveSubdivision;
            const int y = (iPos.fY + sy - margin)/mTileMoveSubdivision;
            bool walkable = true;
            if(x < 0 || x >= mMap->width() ||
               y < 0 || y >= mMap->height()) {
                walkable = false;
            } else {
                const auto& objs = mMap->objects(x, y);
                walkable = objs.empty();
            }
            map.set({sx, sy}, walkable);
        }
    }
    bool found;
    const ePoint from{margin, margin};
    const auto ipos = (pos * subdivision).round();
    const ePoint to{margin + (ipos.fX - iPos.fX),
                    margin + (ipos.fY - iPos.fY)};
    mPath = ePathFinder::findPath(map, from, to, found);
    for(auto& step : mPath) {
        step.fSrc.fX -= margin;
        step.fSrc.fY -= margin;
        step.fDst.fX -= margin;
        step.fDst.fY -= margin;
        step.fSrc.fX /= subdivision;
        step.fSrc.fY /= subdivision;
        step.fDst.fX /= subdivision;
        step.fDst.fY /= subdivision;
        step.fSrc.fX += std::round(mPos.fX*subdivision)/subdivision;
        step.fSrc.fY += std::round(mPos.fY*subdivision)/subdivision;
        step.fDst.fX += std::round(mPos.fX*subdivision)/subdivision;
        step.fDst.fY += std::round(mPos.fY*subdivision)/subdivision;
    }
}

void eGameScreen::paintEvent(ePainter& p) {
    if(!mESCMenu) {
        bool move = false;
        eVec2d vec;
        const double len = 0.1;
        if(mMousePressed) {
            const auto pos = pixelToTilePos(mMousePos);
            vec = eVec2d(pos.fX - mPos.fX,
                         pos.fY - mPos.fY);
            vec.normalize(len);
            const int x = std::floor(mPos.fX + vec.x);
            const int y = std::floor(mPos.fY + vec.y);
            if(x >= 0 && x < mMap->width() &&
               y >= 0 && y < mMap->height()) {
                const auto& objs = mMap->objects(x, y);
                move = objs.empty();
            }
        }
        if(!move) {
            if(mMousePressed) updateTargetPos();
            if(!mPath.empty()) {
                ePathFinderMap map;
                int skipNodes;
                vec = ePathSmoother::moveDir(mPath, map, mPos, 1., skipNodes);
                if(vec.length() > len) vec.normalize(len);
                for(int i = 0; i < skipNodes; i++) {
                    mPath.erase(mPath.begin());
                }
                move = true;
            }
        }
        if(move) {
            mPos.fX += vec.x;
            mPos.fY += vec.y;

            const auto angle = vec.angle();
            mModel.setAngle(angle);
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
        const auto min = pixelToTilePos(mPos.floor(), {0., 0.}).floor();
        const double pdx = mPos.fX - int(mPos.fX);
        const double pdy = mPos.fY - int(mPos.fY);
        const int iMax = terrTypes.size() - 1;
        for(int i = 0; i <= iMax; i++) {
            const auto& terrType = terrTypes[i];
            const auto floor = eTerrsTextures::get(terrType.fName);
            const int dxMax = width()/mTileW + 2;
            const int dyMax = 2*height()/mTileH + 3;
            for(int dy = -1; dy < dyMax; dy++) {
                const int py = (dy + 1)*(mTileH + 1)/2 -
                               std::round((pdx + pdy)*mTileH/2.);
                for(int dx = -1; dx < dxMax; dx++) {
                    const int y = min.fY - dx + dy/2;
                    if(y < 0 || y >= mMap->height()) continue;
                    const int x = min.fX + dx + dy % 2 + dy/2;
                    if(x < 0 || x >= mMap->width()) continue;
                    const auto& tile = mMap->tile(x, y);
                    if(tile.fTerrainType != i) continue;
                    const auto& tex = floor->getTexture(tile.fTileType);
                    const int px = (dy % 2) * mTileW/2 + dx*mTileW -
                                   std::round((pdx - pdy)*mTileW/2.);
                    p.drawTexture(px, py, tex, eAlignment::top | eAlignment::hcenter);
                    const auto iobjs = mMap->objects(x, y);
                    for(const auto& iobj : iobjs) {
                        const auto& obj = mMap->object(iobj);
                        const auto& objType = objTypes[obj.fObjectType];
                        const auto object = eObjsTextures::get(objType.fName);
                        const auto& tex = object->getTexture(obj.fTileType);
                        p.drawTexture(px, py, tex, eAlignment::top | eAlignment::hcenter);
                    }
                }
            }
        }

        p.save();
        p.translate(width()/2, height()/2);
        mModel.draw(p, mFrame);
        p.restore();
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
        updateTargetPos();
    }
    return true;
}

bool eGameScreen::mouseReleaseEvent(const eMouseEvent& e) {
    const auto button = e.button();
    const bool leftReleased = static_cast<bool>(
        button & eMouseButton::left);
    if(leftReleased) {
        mMousePressed = false;
        updateTargetPos();
    }
    return true;
}

bool eGameScreen::mouseMoveEvent(const eMouseEvent& e) {
    mMousePos = ePointF{double(e.x()), double(e.y())};
    const auto buttons = e.buttons();
    const bool leftPressed = static_cast<bool>(
        buttons & eMouseButton::left);
    if(leftPressed) {
        updateTargetPos();
    }
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
    exitB->setPressAction(mExitAction);

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
    mESCMenu->deleteLater();
    mESCMenu = nullptr;
}

#include "egamescreen.h"

#include "../textures/eterrstextures.h"
#include "../textures/echarstextures.h"

#include <eSlayerHelpers/evec2.h>

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
    const ePointF& pixel) {
    ePointF result;
    result.fY = int(mPos.fY) +
                (pixel.fY - height()/2.f)/mTileH +
                (width()/2.f - pixel.fX)/mTileW;
    result.fX = int(mPos.fX) +
                (pixel.fX - width()/2.)/mTileW +
                (pixel.fY - height()/2.)/mTileH;
    return result;
}

void eGameScreen::updateTargetPos() {
    const auto pos = pixelToTilePos(mMousePos);
    setTargetPos(pos);
}

void eGameScreen::setTargetPos(const ePointF& pos) {
    ePathFinder finder;
    const int margin = mPathFindMargin;
    const double subdivision = mTileMoveSubdivision;
    const int dim = 2*margin + 1;
    ePathFinderMap map(dim, dim);
    for(int x = 0; x < dim; x++) {
        for(int y = 0; y < dim; y++) {
            map.set({x, y}, true);
        }
    }
    finder.setMap(map);
    bool found;
    const ePoint from{margin, margin};
    const auto iPos = (mPos * subdivision).round();
    const auto ipos = (pos * subdivision).round();
    const ePoint to{margin + (ipos.fX - iPos.fX),
                    margin + (ipos.fY - iPos.fY)};
    mPath = finder.findPath(from, to, found);
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
    bool move = false;
    const bool canMove = true;
    eVec2d vec;
    const double len = 0.1;
    if(mMousePressed && canMove) {
        const auto pos = pixelToTilePos(mMousePos);
        vec = eVec2d(pos.fX - mPos.fX,
                     pos.fY - mPos.fY);
        vec.normalize(len);
        move = true;
    } else {
        if(mMousePressed) updateTargetPos();
        if(!mPath.empty()) {
            const auto& to = mPath.front().fDst;
            vec = eVec2d(to.fX - mPos.fX, to.fY - mPos.fY);
            if(vec.length() > len) {
                vec.normalize(len);
            } else {
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

    const auto r = renderer();
    {
        const auto holder = mBaseTex->createTargetHolder(r);

        const auto& terrTypes = mMap->terrainTypes();
        const auto min = pixelToTilePos({0., 0.});
        const double pdx = mPos.fX - int(mPos.fX);
        const double pdy = mPos.fY - int(mPos.fY);
        const int iMax = terrTypes.size() - 1;
        for(int i = 0; i <= iMax; i++) {
            const auto& terrType = terrTypes[i];
            const auto floor = eTerrsTextures::get(terrType.fName);
            const int dxMax = width()/mTileW + 1;
            const int dyMax = 2*height()/mTileH + 1;
            for(int dy = 0; dy < dyMax; dy++) {
                const int py = (dy + 1)*(mTileH + 1)/2 -
                               std::round((pdx + pdy)*mTileH/2.);
                for(int dx = 0; dx < dxMax; dx++) {
                    const int y = min.fY - dx + dy/2;
                    if(y < 0 || y >= mMap->height()) continue;
                    const int x = min.fX + dx + dy % 2 + dy/2;
                    if(x < 0 || x >= mMap->width()) continue;
                    const auto& tile = mMap->tile(x, y);
                    if(tile.fTerrainType != i) continue;
                    const auto tex = floor->getTexture(tile.fTileType);
                    const int px = (dy % 2) * mTileW/2 + dx*mTileW - mTileW/2 -
                                   std::round((pdx - pdy)*mTileW/2.);
                    p.drawTexture(px, py, tex);
                }
            }
        }

        p.save();
        p.translate(width()/2, height()/2);
        mModel.draw(p, mFrame++);
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

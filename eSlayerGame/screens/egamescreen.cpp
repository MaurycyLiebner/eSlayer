#include "egamescreen.h"

#include "../textures/eterrstextures.h"
#include "../textures/echarstextures.h"

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

void eGameScreen::paintEvent(ePainter& p) {
    const auto r = renderer();
    {
        const auto holder = mBaseTex->createTargetHolder(r);

        const auto& terrTypes = mMap->terrainTypes();
        const int tileW = 160;
        const int tileH = 79;
        const double yMin = mPosY - height()/2./(tileH) + width()/2./(tileW);
        const double xMin = mPosX - width()/2./(tileW) - height()/2./(tileH);
        // const double yMax = mPosY + height()/2./(tileH) - width()/2./(tileW);
        // const double xMax = mPosX + width()/2./(tileW) + height()/2./(tileH);
        const int iMax = terrTypes.size() - 1;
        for(int i = 0; i <= iMax; i++) {
            const auto& terrType = terrTypes[i];
            const auto floor = eTerrsTextures::get(terrType.fName);
            const int dxMax = width()/tileW + 1;
            const int dyMax = 2*height()/tileH + 1;
            for(int dy = 0; dy < dyMax; dy++) {
                const int py = dy*(tileH + 1)/2;
                for(int dx = 0; dx < dxMax; dx++) {
                    const int y = yMin - dx + dy/2;
                    if(y < 0 || y >= mMap->height()) continue;
                    const int x = xMin + dx + dy % 2 + dy/2;
                    if(x < 0 || x >= mMap->width()) continue;
                    const auto& tile = mMap->tile(x, y);
                    if(tile.fTerrainType != i) continue;
                    const auto tex = floor->getTexture(tile.fTileType);
                    const int px = (dy % 2) * tileW/2 + dx*tileW - tileW/2;
                    p.drawTexture(px, py, tex);
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

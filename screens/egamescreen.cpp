#include "egamescreen.h"

#include "../textures/eterrstextures.h"
#include "../textures/echarstextures.h"

void eGameScreen::initialize() {
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

    // const eCharTextures::eModelParts modelParts {
    //     {"whole", "light"}
    // };
    // const auto texs = eCharsTextures::get("pal");
    const eCharTextures::eModelParts modelParts {
        {"hd", "bare"},
        {"la", "bare"},
        {"lg", "bare"},
        {"ra", "bare"},
        {"tr", "bare"}
    };
    const auto texs = eCharsTextures::get("char");

    const auto model = texs->generateModel(modelParts, r);
    mModel.setCharModel(model);
    mModel.setAnimation(0);
    mModel.setDirection(0);
}

void eGameScreen::paintEvent(ePainter& p) {
    const auto r = renderer();
    {
        const auto holder = mBaseTex->createTargetHolder(r);
        const auto floor = eTerrsTextures::get("town_floor");
        const auto tex = floor->getTexture(5);
        const int w = tex->width();
        const int h = tex->height();
        const int xMax = width()/w;
        const int yMax = 2*height()/h;
        for(int y = 0; y < yMax; y++) {
            const int py = y*(h + 1)/2;
            for(int x = 0; x < xMax; x++) {
                const int px = (y % 2) * w/2 + x*w;
                p.drawTexture(px, py, tex);
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

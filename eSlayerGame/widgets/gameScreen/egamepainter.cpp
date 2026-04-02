#include "egamepainter.h"

std::shared_ptr<eTexture>
eGamePainter::initialize(const int w, const int h) {
    const auto r = renderer();

    mBaseTex = std::make_shared<eTexture>();
    mBaseTex->create(r, w, h, {0, 0, 0, 255});

    mLightingTex = std::make_shared<eLightingTexture>();
    mLightingTex->initialize(r, w, h, SDL_Color{180, 180, 180, 255});

    mDisplayTex = std::make_shared<eTexture>();
    mDisplayTex->create(r, w, h, {0, 0, 0, 255});

    mItemNames = std::make_shared<eTexture>();
    mItemNames->create(r, w, h, {0, 0, 0, 0});

    return mDisplayTex;
}

eRenderTargetHolder eGamePainter::switchToLighting() {
    const auto r = renderer();
    return mLightingTex->createTargetHolder(r);
}

eRenderTargetHolder eGamePainter::switchToBase() {
    const auto r = renderer();
    return mBaseTex->createTargetHolder(r);
}

eRenderTargetHolder eGamePainter::switchToItemNames() {
    const auto r = renderer();
    mRenderItemNames = true;
    return mItemNames->createTargetHolder(r);
}

void eGamePainter::clear() {
    const auto r = renderer();
    mLightingTex->clear(r);
    mBaseTex->fill(r, SDL_Color{0, 0, 0, 255});
    if(mRenderItemNames) {
        mRenderItemNames = false;
        mItemNames->fill(r, SDL_Color{0, 0, 0, 0});
    }
}

void eGamePainter::renderLight(SDL_Renderer* const r,
                               const float x, const float y,
                               const float radius,
                               const SDL_Color& color) {
    mLightingTex->renderLight(r, x, y, radius, color);
}

void eGamePainter::finish() {
    const auto r = renderer();
    const auto holder = mDisplayTex->createTargetHolder(r);
    mBaseTex->setBlendMode(SDL_BLENDMODE_BLEND);
    mBaseTex->render(r, 0, 0);
    mLightingTex->render(r, 0, 0);
    mBaseTex->fill(r, SDL_Color{255, 255, 255, 115});
    mBaseTex->setBlendMode(SDL_BLENDMODE_MUL);
    mBaseTex->render(r, 0, 0);
    if(mRenderItemNames) {
        mItemNames->render(r, 0, 0);
    }
}

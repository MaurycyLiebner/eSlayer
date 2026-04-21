#include "egamepainter.h"

std::shared_ptr<eTexture>
eGamePainter::initialize(const int w, const int h) {
    const auto r = renderer();

    mBaseTex = std::make_shared<eTexture>();
    mBaseTex->create(r, w, h, {0, 0, 0, 255});

    mLightingTex = std::make_shared<eLightingTexture>();
    mLightingTex->initialize(r, w, h, SDL_Color{mLight, mLight, mLight, 255});

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

void eGamePainter::setLightness(const Uint8 light) {
    mLight = light;
    mLightingTex->setClearColor(SDL_Color{light, light, light, 255});
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
    if(mLight == 255) return;
    mLightingTex->renderLight(r, x, y, radius, color);
}

void eGamePainter::finish() {
    const auto r = renderer();
    const auto holder = mDisplayTex->createTargetHolder(r);
    mBaseTex->setBlendMode(SDL_BLENDMODE_BLEND);
    mBaseTex->render(r, 0, 0);
    if(mLight != 255) {
        mLightingTex->render(r, 0, 0);
    }
    const Uint8 a = 255 - mContrast;
    if(a != 255) {
        mBaseTex->fill(r, SDL_Color{255, 255, 255, a});
        mBaseTex->setBlendMode(SDL_BLENDMODE_MUL);
        mBaseTex->render(r, 0, 0);
    }
    if(mRenderItemNames) {
        mItemNames->render(r, 0, 0);
    }
}

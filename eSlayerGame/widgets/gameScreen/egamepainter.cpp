#include "egamepainter.h"

std::shared_ptr<eTexture>
eGamePainter::initialize(const int w, const int h,
                         const int tileW, const int tileH) {
    const auto r = renderer();

    mBaseTex = std::make_shared<eTexture>();
    mBaseTex->create(r, w, h, {0, 0, 0, 255});

    mLightingTex = std::make_shared<eLightingHandler>();
    mLightingTex->initialize(r, w, h, tileW, tileH);

    mDisplayTex = std::make_shared<eTexture>();
    mDisplayTex->create(r, w, h, {0, 0, 0, 255});

    mItemNames = std::make_shared<eTexture>();
    mItemNames->create(r, w, h, {0, 0, 0, 0});

    return mDisplayTex;
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
    mLightingTex->setLightness(light/255.f);
}

void eGamePainter::clear() {
    const auto r = renderer();
    mLightingTex->clear();
    mBaseTex->fill(r, SDL_Color{0, 0, 0, 255});
    if(mRenderItemNames) {
        mRenderItemNames = false;
        mItemNames->fill(r, SDL_Color{0, 0, 0, 0});
    }
    mLights.clear();
}

void eGamePainter::renderLight(const float tx, const float ty,
                               const float x, const float y,
                               const float radius,
                               const SDL_Color& color,
                               const ePaintCall& paintCall) {
    if(mLight == 255) return;
    mLightingTex->addLight(eLight{tx, ty, x, y, radius});
}

void eGamePainter::finish(
    const float tx0, const float ty0,
    const eResolution& res) {
    const auto r = renderer();
    const auto h = mDisplayTex->createTargetHolder(r);
    mBaseTex->setBlendMode(SDL_BLENDMODE_BLEND);
    mBaseTex->render(r, 0, 0);
    {
        mLightingTex->calculateLighting(tx0, ty0);
        mLightingTex->renderFloorLighting(r);
        mLightingTex->renderAll(r);
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

void eGamePainter::addRenderCall(
    const eRenderCallType type,
    const float tx, const float ty,
    const float px, const float py,
    const std::shared_ptr<eTexture>& tex) {
    auto c = std::make_unique<eRenderCall>(
        type, tx, ty, px, py, tex);
    mLightingTex->addRenderCall(c);
}

void eGamePainter::addObjectShadow(
    const float tx, const float ty,
    const float size) {
    auto o = std::make_unique<eObjectLightBlocker>(
        tx, ty, size);
    std::unique_ptr<eBlockerBase> b = std::move(o);
    mLightingTex->addBlocker(b);
}

void eGamePainter::addWallShadow(
    const int tx, const int ty,
    const eWallType dir,
    const float wallMin,
    const float wallMax) {
    auto o = std::make_unique<eWallLightBlocker>(
        tx, ty, dir, wallMin, wallMax);
    std::unique_ptr<eBlockerBase> b = std::move(o);
    mLightingTex->addBlocker(b);
}

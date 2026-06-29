#include "egamepainter.h"

eGamePainter::eGamePainter(
    eTilesIterator& iterator,
    SDL_Renderer* const r) :
    ePainter(r),
    mLightingTex(iterator) {}

std::shared_ptr<eTexture> eGamePainter::initialize(
    const int w, const int h,
    const int tileW, const int tileH) {
    const auto r = renderer();

    mBaseTex = std::make_shared<eTexture>();
    mBaseTex->create(r, w, h, {0, 0, 0, 255});

    mLightingTex.initialize(r, w, h, tileW, tileH);

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
    mLightingTex.setLightness(light/255.f);
}

void eGamePainter::clear() {
    const auto r = renderer();
    mBaseTex->fill(r, SDL_Color{0, 0, 0, 255});
    if(mRenderItemNames) {
        mRenderItemNames = false;
        mItemNames->fill(r, SDL_Color{0, 0, 0, 0});
    }
    mLights.clear();
}

void eGamePainter::addLight(const float tx, const float ty,
                            const float radius) {
    if(mLight == 255) return;
    mLightingTex.addLight(eLight{tx, ty, radius});
}

void eGamePainter::finish(
    const eResolution& res) {
    const auto r = renderer();
    const auto h = mDisplayTex->createTargetHolder(r);
    mBaseTex->setBlendMode(SDL_BLENDMODE_BLEND);
    mBaseTex->render(r, 0, 0);
    {

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

void eGamePainter::calculateAndRenderLighting() {
    const auto r = renderer();
    mLightingTex.calculateLighting();
    mLightingTex.renderFloorLighting(r);
}

void eGamePainter::render(
    const eRenderCall& c) {
    const auto r = renderer();
    mLightingTex.render(r, c);
}

void eGamePainter::addObjectShadow(
    const float tx, const float ty,
    const float size) {
    const std::shared_ptr<eBlockerBase> b =
        std::make_shared<eObjectLightBlocker>(
            tx, ty, size);
    mLightingTex.addBlocker(b);
}

void eGamePainter::addRectShadow(
    const float tx, const float ty,
    const float width, const float height) {
    const std::shared_ptr<eBlockerBase> b =
        std::make_shared<eRectLightBlocker>(
            tx, ty, width, height);
    mLightingTex.addBlocker(b);
}

void eGamePainter::addWallShadow(
    const int tx, const int ty,
    const eWallType dir,
    const float wallMin,
    const float wallMax,
    const bool minFeatherForce,
    const bool maxFeatherForce) {
    if(wallMin == wallMax) return;
    const std::shared_ptr<eBlockerBase> b =
        std::make_unique<eWallLightBlocker>(
        tx, ty, dir, wallMin, wallMax,
        minFeatherForce, maxFeatherForce);
    mLightingTex.addBlocker(b);
}

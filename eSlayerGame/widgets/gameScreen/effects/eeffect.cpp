#include "eeffect.h"

#include "edistorteffect.h"

void eEffects::initialize(
    SDL_Renderer* const r,
    const int w, const int h,
    const float* centerX,
    const float* centerY) {
    mWidth = w;
    mHeight = h;
    mTmp = std::make_shared<eTexture>();
    mTmp->create(r, w, h, {0, 0, 0, 255});
    mCenterX = centerX;
    mCenterY = centerY;
}

void eEffects::apply(SDL_Renderer* const r,
                     std::shared_ptr<eTexture>& to) {
    for(const auto& e : mEffects) {
        e->apply(r, to, mTmp);
    }
}

void eEffects::addEffect(const eEffectSettings& settings) {
    std::shared_ptr<eEffect> e;
    switch(settings.fType) {
    case eEffectType::distort: {
        e = std::make_shared<eDistortEffect>();
    } break;
    }

    mEffects.emplace_back(e);
    e->initialize(settings, mWidth, mHeight, mCenterX, mCenterY);
}

void eEffects::clearEffects() {
    mEffects.clear();
}

void eEffect::initialize(
    const eEffectSettings& settings,
    const int w, const int h,
    const float* centerX,
    const float* centerY) {
    mSettings = settings;
    mWidth = w;
    mHeight = h;
    mCenterX = centerX;
    mCenterY = centerY;
}

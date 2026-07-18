#include "eeffect.h"

#include "edistorteffect.h"
#include "eraineffect.h"
#include "espaceeffect.h"

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
    for(uint8_t i = 0; i < mEffects.size(); i++) {
        const auto& e = mEffects[i];
        if(e->done()) {
            mEffects.erase(mEffects.begin() + i);
            i--;
        } else {
            e->apply(r, to, mTmp);
        }
    }
}

void eEffects::addEffect(const eEffectSettings& settings,
                         const bool fadeIn) {
    std::shared_ptr<eEffect> e;
    switch(settings.fType) {
    case eEffectType::distort: {
        e = std::make_shared<eDistortEffect>();
    } break;
    case eEffectType::rain: {
        e = std::make_shared<eRainEffect>();
    } break;
    case eEffectType::space: {
        e = std::make_shared<eSpaceEffect>();
    } break;
    }

    mEffects.emplace_back(e);
    e->initialize(settings, mWidth, mHeight,
                  mCenterX, mCenterY, fadeIn);
}

void eEffects::clearEffects() {
    for(const auto& e : mEffects) {
        e->stop();
    }
}

void eEffect::initialize(
    const eEffectSettings& settings,
    const int w, const int h,
    const float* centerX,
    const float* centerY,
    const bool fadeIn) {
    mSettings = settings;
    mWidth = w;
    mHeight = h;
    mCenterX = centerX;
    mCenterY = centerY;
}

void eEffect::stop() {
    mDone = true;
}

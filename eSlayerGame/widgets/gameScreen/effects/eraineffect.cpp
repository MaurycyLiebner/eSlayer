#include "eraineffect.h"

#include <eSlayerHelpers/erand.h>

void eRainEffect::apply(
    SDL_Renderer* const r,
    std::shared_ptr<eTexture>& to,
    std::shared_ptr<eTexture>& tmp) {
    const auto h = to->createTargetHolder(r);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_RenderGeometry(r, nullptr,
                       mVerts.data(), mVerts.size(),
                       mIndices.data(), mIndices.size());
    increment(1.f);
}

void eRainEffect::initialize(
    const eEffectSettings& settings,
    const int w, const int h,
    const float* centerX,
    const float* centerY) {
    eEffect::initialize(settings, w, h, centerX, centerY);

    mSpeed = 0.025f*settings.fSpeed;
    mCount = 500*settings.fScale;

    mVerts.reserve(4*mCount);
    mIndices.reserve(6*mCount);

    const SDL_FColor color{1.f, 1.f, 1.f, 0.25f};

    for(uint16_t i = 0; i < mCount; i++) {
        mX0.emplace_back(eRand::randF(-0.1f*mWidth, 1.1f*mWidth));
        mY0.emplace_back(eRand::randF(-mHeight, 0.f));

        const int ishift = mVerts.size();
        {
            auto& v = mVerts.emplace_back();
            v.color = color;
        }
        {
            auto& v = mVerts.emplace_back();
            v.color = color;
        }
        {
            auto& v = mVerts.emplace_back();
            v.color = color;
        }
        {
            auto& v = mVerts.emplace_back();
            v.color = color;
        }
        mIndices.emplace_back(ishift);
        mIndices.emplace_back(ishift + 1);
        mIndices.emplace_back(ishift + 2);
        mIndices.emplace_back(ishift);
        mIndices.emplace_back(ishift + 2);
        mIndices.emplace_back(ishift + 3);
    }

    mX = mX0;
    mY = mY0;
}

void eRainEffect::stop() {
    mFade = true;
}

void eRainEffect::increment(const float by) {
    mTime += by;

    mDone = mFade;

    const float plen = mLength*mHeight;
    const float pthick = mThick*mHeight;
    for(uint16_t i = 0; i < mCount; i++) {
        float& x = mX[i];
        float& y = mY[i];
        const uint16_t v0i = 4*i;
        auto& v0 = mVerts[v0i];
        auto& v1 = mVerts[v0i + 1];
        auto& v2 = mVerts[v0i + 2];
        auto& v3 = mVerts[v0i + 3];

        const float yInc = mHeight*by*mSpeed;
        y += yInc;
        if(y > mHeight && !mFade) {
            y = mY0[i];
            x = mX0[i];
        } else {
            x += yInc*mTilt;
            mDone = false;
        }

        v0.position = {x, y};
        v1.position = {x + pthick, y};
        const float dx = mTilt*plen;
        v2.position = {x + pthick + dx, y + plen};
        v3.position = {x + dx, y + plen};
    }
}
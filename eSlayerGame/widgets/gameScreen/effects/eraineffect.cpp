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
    const float* centerY,
    const bool fadeIn) {
    eEffect::initialize(settings, w, h, centerX, centerY, fadeIn);

    mSpeed = 0.025f*settings.fSpeed;
    mCount = 500*settings.fScale;

    mVerts.reserve(4*mCount);
    mIndices.reserve(6*mCount);

    const SDL_FColor color{1.f, 1.f, 1.f, 0.25f};

    const float minX = -mTilt*mWidth;
    const float maxX = (1.f + mTilt)*mWidth;
    const float minY = fadeIn ? -mHeight : 0.f;
    const float maxY = fadeIn ? 0.f : mHeight;
    if(!fadeIn) mTime = mCount;
    for(uint16_t i = 0; i < mCount; i++) {
        const float x0 = eRand::randF(minX, maxX);
        const float y0 = eRand::randF(minY, maxY);

        mX0.emplace_back(x0);
        mY.emplace_back(y0);

        const uint16_t v0i = 4*i;

        auto& v0 = mVerts.emplace_back();
        v0.color = color;
        v0.position = {x0, y0};
        auto& v1 = mVerts.emplace_back();
        v1.color = color;
        v1.position = {x0, y0};
        auto& v2 = mVerts.emplace_back();
        v2.color = color;
        v2.position = {x0, y0};
        auto& v3 = mVerts.emplace_back();
        v3.color = color;
        v3.position = {x0, y0};

        mIndices.emplace_back(v0i);
        mIndices.emplace_back(v0i + 1);
        mIndices.emplace_back(v0i + 2);
        mIndices.emplace_back(v0i);
        mIndices.emplace_back(v0i + 2);
        mIndices.emplace_back(v0i + 3);
    }

    mX = mX0;
}

void eRainEffect::stop() {
    if(mFade) return;
    mFade = true;
    mRemTime = mCount/mFadeSpeed;
}

void eRainEffect::increment(const float by) {
    mTime += by;
    if(mFade) mRemTime -= by;
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

        if(i > mTime) break;

        const float yInc = mHeight*by*mSpeed;
        y += yInc;
        if(y > mHeight) {
            if(!mFade || i < mFadeSpeed*mRemTime) {
                y = 0.f;
                x = mX0[i];
            }
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
#include "espaceeffect.h"

#include <eSlayerHelpers/erand.h>

void eSpaceEffect::apply(
    SDL_Renderer* const r,
    std::shared_ptr<eTexture>& to,
    std::shared_ptr<eTexture>& tmp) {
    tmp->fill(r, SDL_Color{0, 0, 0, 0});
    const auto h = tmp->createTargetHolder(r);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_RenderGeometry(r, nullptr,
                       mVerts.data(), mVerts.size(),
                       mIndices.data(), mIndices.size());
    to->setBlendMode(SDL_BLENDMODE_BLEND);
    to->render(r, 0, 0);
    std::swap(to, tmp);
    increment(1.f);
}

void eSpaceEffect::initialize(
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

    const SDL_FColor color{1.f, 1.f, 1.f, 1.f};

    const float minX = fadeIn ? -mWidth : 0.f;
    const float maxX = fadeIn ? 0.f : mWidth;
    for(uint16_t i = 0; i < mCount; i++) {
        mSpeed0.emplace_back(eRand::randF(0.1f, 1.f));

        mX.emplace_back(eRand::randF(minX, maxX));
        mY.emplace_back(eRand::randF(0.f, mHeight));

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
}

void eSpaceEffect::stop() {
    mFade = true;
}

void eSpaceEffect::increment(const float by) {
    mTime += by;

    mDone = mFade;

    const float size = mSize*mHeight;
    for(uint16_t i = 0; i < mCount; i++) {
        float& x = mX[i];
        const float& y = mY[i];
        const uint16_t v0i = 4*i;
        auto& v0 = mVerts[v0i];
        auto& v1 = mVerts[v0i + 1];
        auto& v2 = mVerts[v0i + 2];
        auto& v3 = mVerts[v0i + 3];

        const float xInc = mHeight*by*mSpeed*mSpeed0[i];
        x += xInc;
        if(x > mWidth && !mFade) {
            x = 0.f;
        } else {
            mDone = false;
        }

        v0.position = {x, y};
        v1.position = {x + size, y};
        v2.position = {x + size, y + size};
        v3.position = {x, y + size};
    }
}
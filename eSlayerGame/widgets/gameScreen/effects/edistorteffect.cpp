#include "edistorteffect.h"

#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/epoint.h>

void eDistortEffect::apply(
    SDL_Renderer* const r,
    std::shared_ptr<eTexture>& to,
    std::shared_ptr<eTexture>& tmp) {
    const auto h = tmp->createTargetHolder(r);
    to->setBlendMode(SDL_BLENDMODE_BLEND);
    const ePointF c{(*mCenterX)*mWidth, (*mCenterY)*mHeight};
    for(uint16_t i = 0; i < mNVerts; i++) {
        const float x = mXTexCoords[i];
        const float y = mYTexCoords[i];
        if(x == 0.f) continue;
        if(x == 1.f) continue;
        if(y == 0.f) continue;
        if(y == 1.f) continue;
        const float xr = (1.f - mTime)*mXR1[i] + mTime*mXR2[i];
        const float yr = (1.f - mTime)*mYR1[i] + mTime*mYR2[i];
        const ePointF p{x*mWidth, y*mHeight};
        const float dist = ePointF::distance(c, p);
        const float infl = std::clamp(4.f*(dist/mHeight - 0.25f), 0.f, 1.f);
        mVerts[i].tex_coord = {x + infl*xr, y + infl*yr};
    }
    const auto sdlTex = to->tex();
    SDL_RenderGeometry(r, sdlTex,
                       mVerts.data(), mVerts.size(),
                       mIndices.data(), mIndices.size());
    std::swap(tmp, to);

    increment(1.f);
}

void eDistortEffect::initialize(
    const eEffectSettings& settings,
    const int w, const int h,
    const float* centerX,
    const float* centerY) {
    eEffect::initialize(settings, w, h, centerX, centerY);

    mSpeed = 0.025f*settings.fSpeed;
    mScale = 0.025f*settings.fScale;

    mYDiv = 10;
    mXDiv = (mYDiv*w + h/2)/h;

    const uint16_t nIndices = 6*(mYDiv - 1)*(mXDiv - 1);
    mIndices.reserve(nIndices);

    for(uint16_t ix = 0; ix < mXDiv - 1; ix++) {
        for(uint16_t iy = 0; iy < mYDiv - 1; iy++) {
            const int v0 = ix * mYDiv + iy;
            const int v1 = (ix + 1) * mYDiv + iy;
            const int v2 = (ix + 1) * mYDiv + (iy + 1);
            const int v3 = ix * mYDiv + (iy + 1);

            mIndices.emplace_back(v0);
            mIndices.emplace_back(v1);
            mIndices.emplace_back(v2);

            mIndices.emplace_back(v0);
            mIndices.emplace_back(v2);
            mIndices.emplace_back(v3);
        }
    }

    mNVerts = mYDiv*mXDiv;
    mVerts.reserve(mNVerts);

    for(uint16_t ix = 0; ix < mXDiv; ix++) {
        float x;
        if(ix == 0) {
            x = 0.f;
        } else if(ix == mXDiv - 1) {
            x = 1.f;
        } else {
            x = 1.f*ix/(mXDiv - 1);
        }
        for(uint16_t iy = 0; iy < mYDiv; iy++) {
            float y;
            if(iy == 0) {
                y = 0.f;
            } else if(iy == mYDiv - 1) {
                y = 1.f;
            } else {
                y = 1.f*iy/(mYDiv - 1);
            }
            mXTexCoords.emplace_back(x);
            mYTexCoords.emplace_back(y);
            auto& v = mVerts.emplace_back();
            v.color = {1.f, 1.f, 1.f, 1.f};
            v.position = {x*w, y*h};
            v.tex_coord = {x, y};
        }
    }

    mXR1 = std::vector<float>(mNVerts, 0.f);
    mYR1 = std::vector<float>(mNVerts, 0.f);
    mXR2 = generateRandomMap();
    mYR2 = generateRandomMap();
}

void eDistortEffect::stop() {
    mFade = true;
}

void eDistortEffect::generateRandomMap(
    std::vector<float>& result) const {
    result.clear();
    for(uint16_t i = 0; i < mNVerts; i++) {
        result.emplace_back(eRand::randF(0.f, mScale));
    }
}

std::vector<float> eDistortEffect::generateRandomMap() const {
    std::vector<float> result;
    result.reserve(mNVerts);
    generateRandomMap(result);
    return std::move(result);
}

void eDistortEffect::increment(const float by) {
    mTime += mSpeed*by;
    if(mTime > 1.f) {
        mTime = mTime - 1.f;

        std::swap(mXR1, mXR2);
        std::swap(mYR1, mYR2);

        if(mFade) {
            if(mFading) mDone = true;
            mFading = true;
            mXR2 = std::vector<float>(mNVerts, 0.f);
            mYR2 = std::vector<float>(mNVerts, 0.f);
        } else {
            generateRandomMap(mXR2);
            generateRandomMap(mYR2);
        }
    }
}

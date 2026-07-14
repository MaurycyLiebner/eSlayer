#include "edistorteffect.h"

#include <eSlayerHelpers/erand.h>

void eDistortEffect::apply(
    SDL_Renderer* const r,
    std::shared_ptr<eTexture>& to,
    std::shared_ptr<eTexture>& tmp) {
    const auto h = tmp->createTargetHolder(r);
    to->setBlendMode(SDL_BLENDMODE_BLEND);
    const auto sdlTex = to->tex();
    SDL_RenderGeometry(r, sdlTex,
                       mVerts.data(), mVerts.size(),
                       mIndices.data(), mIndices.size());
    std::swap(tmp, to);
}

void eDistortEffect::initialize(
    const int w, const int h) {
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

    const uint16_t nVerts = mYDiv*mXDiv;
    mVerts.reserve(nVerts);

    for(uint16_t ix = 0; ix < mXDiv; ix++) {
        float x;
        float rx;
        if(ix == 0) {
            x = 0.f;
            rx = 0.f;
        } else if(ix == mXDiv - 1) {
            x = 1.f;
            rx = 1.f;
        } else {
            x = 1.f*ix/(mXDiv - 1);
            rx = x + eRand::randF(0.f, 0.25f/mXDiv);
        }
        for(uint16_t iy = 0; iy < mYDiv; iy++) {
            float y;
            float ry;
            if(iy == 0) {
                y = 0.f;
                ry = 0.f;
            } else if(iy == mYDiv - 1) {
                y = 1.f;
                ry = 1.f;
            } else {
                y = 1.f*iy/(mYDiv - 1);
                ry = y + eRand::randF(0.f, 0.25f/mYDiv);
            }
            auto& v = mVerts.emplace_back();
            v.color = {1.f, 1.f, 1.f, 1.f};
            v.position = {x*w, y*h};
            v.tex_coord = {rx, ry};
        }
    }
}

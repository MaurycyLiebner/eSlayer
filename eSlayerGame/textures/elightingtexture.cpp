#include "elightingtexture.h"

#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/epoint.h>

#include <cmath>

void eLightingTexture::initialize(SDL_Renderer * const r,
                                  const int w, const int h,
                                  const int tileW, const int tileH,
                                  const SDL_Color& color) {
    create(r, w, h, color);
    mColor = color;
    setBlendMode(SDL_BLENDMODE_MOD);

    mBaseTileW = tileW;
    mBaseTileH = tileH;

    mFloorLightW = std::round(sTileDimMult*mBaseTileW);
    mFloorLightH = std::round(sTileDimMult*mBaseTileH);

    mNRows = (2*h + mFloorLightH - 1)/mFloorLightH;
    mNCols = (w + mFloorLightW - 1)/mFloorLightW;

    mFloorLighting = std::vector<std::vector<float>>(
        mNRows, std::vector<float>(mNCols, 0.f));
}

void eLightingTexture::setClearColor(const SDL_Color& color) {
    mColor = color;
}

void eLightingTexture::clear(SDL_Renderer * const r) {
    fill(r, mColor);
    mLights.clear();
}

void eLightingTexture::addLight(const eLight& light) {
    mLights.emplace_back(light);
}

void eLightingTexture::addBlocker(std::unique_ptr<eBlockerBase>& b) {
    mBlockers.emplace_back(std::move(b));
}

void rectToIso(const int x, const int y,
               int& tx, int& ty) {
    tx = x + y % 2 + y/2;
    ty = -x + y/2;
}

void eLightingTexture::calculateFloorLighting(
    const float tx0, const float ty0) {
    for(int x = 0; x < mNCols; x++) {
        for(int y = 0; y < mNRows; y++) {
            float& v = mFloorLighting[y][x];
            v = 0.f;
            int tx;
            int ty;
            rectToIso(x, y, tx, ty);
            for(const auto& l : mLights) {
                const float lr = l.fRadius*sTileDimMultInv;
                const float ltx = (l.fTX - tx0)*sTileDimMultInv;
                const float lty = (l.fTY - ty0)*sTileDimMultInv;
                const float dx = tx - ltx;
                const float dy = ty - lty;
                const float distSq = dx*dx + dy*dy;
                const float i = 1.f/(1.f + 0.01f*distSq);
                v = std::max(v, i);
            }
        }
    }
}

ePointF rectTileToPixel(const int x, const int y,
                        const int tileW, const int tileH) {
    ePointF result;
    result.fX = x*tileW + (y % 2) * (0.5f*tileW);
    result.fY = 0.5f*y*tileH;
    return result;
}

void eLightingTexture::renderFloorLighting(
    SDL_Renderer * const r) {
    const auto h = createTargetHolder(r);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    auto make = [](const ePointF& p, const float i) {
        SDL_Vertex v;
        v.position = {p.fX, p.fY};
        v.color = SDL_FColor{1.f, 1.f, 1.f, i};
        v.tex_coord = {0.f, 0.f};
        return v;
    };

    for(int x = 1; x < mNCols - 1; x++) {
        for(int y = 1; y < mNRows - 2; y++) {
            const int topX = x;
            const int topY = y;
            const float topV = mFloorLighting[topY][topX];
            const int rightX = topX + topY % 2;
            const int rightY = topY + 1;
            const float rightV = mFloorLighting[rightY][rightX];
            const int bottomX = topX;
            const int bottomY = topY + 2;
            const float bottomV = mFloorLighting[bottomY][bottomX];
            const int leftX = rightX - 1;
            const int leftY = rightY;
            const float leftV = mFloorLighting[leftY][leftX];

            const auto topPos = rectTileToPixel(topX, topY, mFloorLightW, mFloorLightH);
            const auto topVert = make(topPos, topV);
            const auto rightPos = rectTileToPixel(rightX, rightY, mFloorLightW, mFloorLightH);
            const auto rightVert = make(rightPos, rightV);
            const auto bottomPos = rectTileToPixel(bottomX, bottomY, mFloorLightW, mFloorLightH);
            const auto bottomVert = make(bottomPos, bottomV);
            const auto leftPos = rectTileToPixel(leftX, leftY, mFloorLightW, mFloorLightH);
            const auto leftVert = make(leftPos, leftV);

            std::vector<SDL_Vertex> verts;
            verts.reserve(6);
            verts.emplace_back(topVert);
            verts.emplace_back(rightVert);
            verts.emplace_back(bottomVert);
            verts.emplace_back(topVert);
            verts.emplace_back(bottomVert);
            verts.emplace_back(leftVert);
            SDL_RenderGeometry(r, nullptr, verts.data(),
                               verts.size(), nullptr, 0);
        }
    }
}

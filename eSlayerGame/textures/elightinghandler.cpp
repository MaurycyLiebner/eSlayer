#include "elightinghandler.h"

#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/epoint.h>

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <cmath>

void eLightingHandler::initialize(SDL_Renderer * const r,
                                  const int w, const int h,
                                  const int tileW, const int tileH) {
    mBaseTileW = tileW;
    mBaseTileH = tileH;

    mFloorLightW = std::round(sTileDimMult*mBaseTileW);
    mFloorLightH = std::round(sTileDimMult*mBaseTileH);

    mNRows = (2*h + mFloorLightH - 1)/mFloorLightH;
    mNCols = (w + mFloorLightW - 1)/mFloorLightW;

    mFloorLighting = std::vector<std::vector<float>>(
        mNRows, std::vector<float>(mNCols, 0.f));
}

void eLightingHandler::setLightness(const float l) {
    mLightness = l;
}

void eLightingHandler::clear() {
    mLights.clear();
    mBlockers.clear();
    mRenderCalls.clear();
}

void eLightingHandler::addLight(const eLight& light) {
    mLights.emplace_back(light);
}

void eLightingHandler::addBlocker(std::unique_ptr<eBlockerBase>& b) {
    mBlockers.emplace_back(std::move(b));
}

void rectToIso(const int x, const int y,
               int& tx, int& ty) {
    tx = x + y % 2 + y/2;
    ty = -x + y/2;
}

void eLightingHandler::calculateLighting(
    const float tx0, const float ty0) {
    for(int x = 0; x < mNCols; x++) {
        for(int y = 0; y < mNRows; y++) {
            float& v = mFloorLighting[y][x];
            v = mLightness;
            int tx;
            int ty;
            rectToIso(x, y, tx, ty);
            for(const auto& l : mLights) {
                const float lr = l.fRadius*sTileDimMultInv;
                const float ltx = (l.fTX - tx0)*sTileDimMultInv;
                const float lty = (l.fTY - ty0)*sTileDimMultInv;

                bool blocked = false;
                for(const auto& b : mBlockers) {
                    const auto& bref = *b;
                    switch(bref.fType) {
                    case eBlockerBaseType::object: {
                        const auto& oref = static_cast<const eObjectLightBlocker&>(bref);
                        const float s = oref.fSize*sTileDimMultInv;
                        const float otx = (bref.fTX - tx0)*sTileDimMultInv;
                        const float oty = (bref.fTY - ty0)*sTileDimMultInv;
                        const SDL_FRect rect{otx, oty, s, s};
                        float x1 = tx;
                        float y1 = ty;
                        float x2 = ltx;
                        float y2 = lty;
                        const bool r = SDL_GetRectAndLineIntersectionFloat(&rect, &x1, &y1, &x2, &y2);
                        if(r) blocked = true;
                    } break;
                    case eBlockerBaseType::wall: {

                    } break;
                    }
                }
                if(blocked) continue;

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

void eLightingHandler::renderFloorLighting(SDL_Renderer * const r) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_MUL);

    auto make = [](const ePointF& p, const float i) {
        SDL_Vertex v;
        v.position = {p.fX, p.fY};
        v.color = SDL_FColor{i, i, i, 1.f};
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

void eLightingHandler::addRenderCall(
    std::unique_ptr<eRenderCall>& c) {
    mRenderCalls.emplace_back(std::move(c));
}

void render(SDL_Renderer* const r,
            const float x, const float y,
            const std::shared_ptr<eTexture>& tex,
            const std::vector<float>& lightness) {
    const int nStrips = lightness.size() - 1;
    if(nStrips < 1) return;
    const float vTexCoordW = 1.f/nStrips;
    const int tw = tex->width();
    const int th = tex->height();
    const float vPosW = vTexCoordW*tw;

    std::vector<SDL_Vertex> verts;
    const int nVerts = 4*nStrips;
    verts.reserve(nVerts);

    std::vector<int> indices;
    const int nIndices = 6*nStrips;
    indices.reserve(nIndices);

    const auto sprite = tex->sprite();

    for(int s = 0; s < nStrips; s++) {
        {
            indices.emplace_back(verts.size());
            auto& tl = verts.emplace_back();
            const float l = lightness[s];
            tl.color = SDL_FColor{l, l, l, 1.f};
            tl.position.x = x + s*vPosW;
            tl.position.y = y;
            tl.tex_coord.x = s*vTexCoordW;
            tl.tex_coord.y = 0.f;
            sprite.mapCoords(tl.tex_coord);
        }
        {
            indices.emplace_back(verts.size());
            auto& tr = verts.emplace_back();
            const float l = lightness[s + 1];
            tr.color = SDL_FColor{l, l, l, 1.f};
            tr.position.x = x + (s + 1)*vPosW;
            tr.position.y = y;
            tr.tex_coord.x = (s + 1)*vTexCoordW;
            tr.tex_coord.y = 0.f;
            sprite.mapCoords(tr.tex_coord);
        }
        {
            indices.emplace_back(verts.size());
            auto& br = verts.emplace_back();
            const float l = lightness[s + 1];
            br.color = SDL_FColor{l, l, l, 1.f};
            br.position.x = x + (s + 1)*vPosW;
            br.position.y = y + th;
            br.tex_coord.x = (s + 1)*vTexCoordW;
            br.tex_coord.y = 1.f;
            sprite.mapCoords(br.tex_coord);
        }
        {
            indices.emplace_back(verts.size());
            indices.emplace_back(verts.size() - 1);
            indices.emplace_back(verts.size() - 3);
            auto& bl = verts.emplace_back();
            const float l = lightness[s];
            bl.color = SDL_FColor{l, l, l, 1.f};
            bl.position.x = x + s*vPosW;
            bl.position.y = y + th;
            bl.tex_coord.x = s*vTexCoordW;
            bl.tex_coord.y = 1.f;
            sprite.mapCoords(bl.tex_coord);
        }
    }

    const auto stex = sprite.fTex;
    const auto sdlTex = stex->tex();
    SDL_RenderGeometry(r, sdlTex,
                       verts.data(), verts.size(),
                       indices.data(), indices.size());
}

void eLightingHandler::renderAll(SDL_Renderer* const r) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    for(const auto& c : mRenderCalls) {
        const auto& cref = *c;
        const auto type = c->fType;
        std::vector<float> lightness;
        switch(type) {
        case eRenderCallType::object: {
            lightness.emplace_back(1.f);
            lightness.emplace_back(1.f);
        } break;
        }
        render(r, cref.fX, cref.fY, cref.fTex, lightness);
    }
}

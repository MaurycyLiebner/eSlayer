#include "elightinghandler.h"

#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/epoint.h>

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <cmath>

void eLightingHandler::initialize(SDL_Renderer * const r,
                                  const int w, const int h,
                                  const int tileW, const int tileH) {
    mFeatherLen = 0.5f*sTileDimMultInv;

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
    const float r = light.fRadius*sTileDimMultInv;
    const float tx = (light.fTX - mTopLeft.fX)*sTileDimMultInv;
    const float ty = (light.fTY - mTopLeft.fY)*sTileDimMultInv;
    mLights.emplace_back(tx, ty, r);
}

void eLightingHandler::addBlocker(std::unique_ptr<eBlockerBase>& b) {
    b->fTX = (b->fTX - mTopLeft.fX)*sTileDimMultInv;
    b->fTY = (b->fTY - mTopLeft.fY)*sTileDimMultInv;
    if(b->fType == eBlockerBaseType::object) {
        auto& o = static_cast<eObjectLightBlocker&>(*b);
        o.fSize = o.fSize*sTileDimMultInv;
    }
    mBlockers.emplace_back(std::move(b));
}

void rectToIso(const int x, const int y,
               int& tx, int& ty) {
    tx = x + y % 2 + y/2;
    ty = -x + y/2;
}

void isoToRect(const int tx, const int ty,
               int& x, int& y) {
    y = tx + ty;
    x = tx - (y % 2) - y / 2;
}

bool lineIntersection(
    const ePointF& p1, const ePointF& p2,
    const ePointF& p3, const ePointF& p4,
    ePointF* const out = nullptr) {
    const float x1 = p1.fX;
    const float y1 = p1.fY;
    const float x2 = p2.fX;
    const float y2 = p2.fY;

    const float x3 = p3.fX;
    const float y3 = p3.fY;
    const float x4 = p4.fX;
    const float y4 = p4.fY;

    const float denom = (x1 - x2) * (y3 - y4) -
                        (y1 - y2) * (x3 - x4);

    // Parallel lines
    if(std::fabs(denom) < 0.0001f) return false;

    const float t =
        ((x1 - x3) * (y3 - y4) -
         (y1 - y3) * (x3 - x4)) / denom;

    const float u =
        ((x1 - x3) * (y1 - y2) -
         (y1 - y3) * (x1 - x2)) / denom;

    // Segment intersection test
    if(t < 0.0f || t > 1.0f ||
       u < 0.0f || u > 1.0f) {
        return false;
    }

    if(out) {
        out->fX = x1 + t * (x2 - x1);
        out->fY = y1 + t * (y2 - y1);
    }

    return true;
}

void eLightingHandler::calculateLighting() {
    for(int y = 0; y < mNRows; y++) {
        for(int x = 0; x < mNCols; x++) {
            float& v = mFloorLighting[y][x];
            v = mLightness;
            int tx;
            int ty;
            rectToIso(x, y, tx, ty);
            const ePoint tp{tx, ty};
            for(const auto& l : mLights) {
                const ePointF lp{l.fTX, l.fTY};
                eVec2f dir = ePointF::vector(tp, lp);
                const float dist = dir.length();
                if(dist < 0.001f) {
                    v = 1.f;
                    break;
                }
                if(dist > l.fRadius) continue;
                dir = dir/dist;
                const eVec2f perp(-dir.y, dir.x);

                float mult = 1.f;
                for(const auto& b : mBlockers) {
                    const auto& bref = *b;
                    switch(bref.fType) {
                    case eBlockerBaseType::object: {
                        const auto& oref = static_cast<const eObjectLightBlocker&>(bref);
                        const float s = oref.fSize;
                        const ePointF oc{bref.fTX + 0.5f*s, bref.fTY + 0.5f*s};
                        const ePointF o1 = oc + perp*0.5f*s;
                        const ePointF o2 = oc - perp*0.5f*s;
                        ePointF inters;
                        const bool r = lineIntersection(tp, lp, o1, o2, &inters);
                        if(r) {
                            const float dist = ePointF::distance(oc, inters);
                            mult = std::min(mult, 1.f - std::clamp((0.5f*s - dist)/mFeatherLen, 0.f, 1.f));
                        }
                    } break;
                    case eBlockerBaseType::wall: {

                    } break;
                    }
                }

                const float dx = tx - l.fTX;
                const float dy = ty - l.fTY;
                const float distSq = dx*dx + dy*dy;
                const float t = sqrt(distSq)/l.fRadius;
                const float i = 1.f - t*t;
                v = std::max(v, i*mult);
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

    std::vector<SDL_Vertex> verts;
    verts.reserve(6*mNCols*mNRows);
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

            verts.emplace_back(topVert);
            verts.emplace_back(rightVert);
            verts.emplace_back(bottomVert);
            verts.emplace_back(topVert);
            verts.emplace_back(bottomVert);
            verts.emplace_back(leftVert);
        }
    }
    SDL_RenderGeometry(r, nullptr, verts.data(),
                       verts.size(), nullptr, 0);
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
            float l = 1.f;
            const float ctx = (cref.fTX - mTopLeft.fX)*sTileDimMultInv;
            const float cty = (cref.fTY - mTopLeft.fY)*sTileDimMultInv;
            const int ictx = std::round(ctx);
            const int icty = std::round(cty);
            int rtx;
            int rty;
            isoToRect(ictx, icty, rtx, rty);
            if(rtx >= 0 && rty >= 0 &&
               rtx < mNCols && rty < mNRows) {
                l = mFloorLighting[rty][rtx];
            }
            lightness.emplace_back(l);
            lightness.emplace_back(l);
        } break;
        }
        render(r, cref.fX, cref.fY, cref.fTex, lightness);
    }
}

void eLightingHandler::setTopLeftTilePos(const ePointF& pos) {
    mTopLeft = pos;
}

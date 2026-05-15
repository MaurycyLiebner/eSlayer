#include "elightinghandler.h"

#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/epoint.h>

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <cmath>

eLightingHandler::eLightingHandler(
    eTilesIterator& tileIterator) :
    mIterator(tileIterator) {}

void eLightingHandler::initialize(SDL_Renderer* const r,
                                  const int w, const int h,
                                  const int tileW,
                                  const int tileH) {
    mFeatherLen = 0.5f;

    mBaseTileW = tileW;
    mBaseTileH = tileH;
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
    mIterator.iterate([&](eTileInfo& tile) {
        auto& lighting = tile.fLighting;
        lighting.resize(sNDots*sNDots);
        for(int y = 0; y < sNDots; y++) {
            const float ty = tile.fTY + float(y)/sTileDiv;
            for(int x = 0; x < sNDots; x++) {
                const float tx = tile.fTX + float(x)/sTileDiv;
                float& v = lighting[y*sNDots + x];
                v = mLightness;
                const ePointF tp{tx, ty};
                for(const auto& l : mLights) {
                    const ePointF lp{l.fTX, l.fTY};
                    eVec2f dir = ePointF::vector(tp, lp);
                    const float dist = dir.length();
                    if(dist < 0.001f) break;
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
                            const auto& wref = static_cast<const eWallLightBlocker&>(bref);
                            ePointF p1;
                            ePointF p2;
                            switch(wref.fDir) {
                            case eWallType::topLeft: {
                                p1.fX = wref.fTX;
                                p1.fY = wref.fTY + wref.fWallMin;
                                p2.fX = wref.fTX;
                                p2.fY = wref.fTY + wref.fWallMax;
                            } break;
                            case eWallType::topRight: {
                                p1.fX = wref.fTX + wref.fWallMin;
                                p1.fY = wref.fTY;
                                p2.fX = wref.fTX + wref.fWallMax;
                                p2.fY = wref.fTY;
                            } break;
                            }
                            ePointF inters;
                            const bool r = lineIntersection(tp, lp, p1, p2, &inters);
                            if(r) {
                                mult = 0.f;
                            }
                        } break;
                        }
                    }

                    const float dx = tx - l.fTX;
                    const float dy = ty - l.fTY;
                    const float distSq = dx*dx + dy*dy;
                    const float t = sqrt(distSq)/l.fRadius;
                    const float i = 1.f - t*t*t*t;
                    v = std::max(v, i*mult);
                }
            }
        }
    });
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

    const auto make = [](const ePointF& p, const float i) {
        SDL_Vertex v;
        v.position = {p.fX, p.fY};
        v.color = SDL_FColor{i, i, i, 1.f};
        v.tex_coord = {0.f, 0.f};
        return v;
    };

    const auto shift = [&](ePointF& p,
                           const float dtx,
                           const float dty) {
        p.fX += (dtx - dty)*(mBaseTileW/2);
        p.fY += (dtx + dty)*(mBaseTileH + 1)/2;
    };

    std::vector<SDL_Vertex> verts;
    const int c = mIterator.tileCount();
    verts.reserve(6*c*sTileDiv*sTileDiv);

    mIterator.iterate([&](const eTileInfo& tile) {
        const float singleShift = 1.f/sTileDiv;
        const auto& lighting = tile.fLighting;
        for(int y = 0; y < sTileDiv; y++) {
            const float dty = float(y)/sTileDiv;
            const float ty = tile.fTY + dty;
            for(int x = 0; x < sTileDiv; x++) {
                const float dtx = float(x)/sTileDiv;
                const float tx = tile.fTX + dtx;

                const float topV = lighting[y*sNDots + x];
                const float rightV = lighting[y*sNDots + x + 1];
                const float bottomV = lighting[(y + 1)*sNDots + x + 1];
                const float leftV = lighting[(y + 1)*sNDots + x];

                auto topPos = ePointF{tile.fPX, tile.fPY};
                shift(topPos, dtx, dty);
                const auto topVert = make(topPos, topV);

                auto rightPos = ePointF{tile.fPX, tile.fPY};
                shift(rightPos, dtx + singleShift, dty);
                const auto rightVert = make(rightPos, rightV);

                auto bottomPos = ePointF{tile.fPX, tile.fPY};
                shift(bottomPos, dtx + singleShift, dty + singleShift);
                const auto bottomVert = make(bottomPos, bottomV);

                auto leftPos = ePointF{tile.fPX, tile.fPY};
                shift(leftPos, dtx, dty + singleShift);
                const auto leftVert = make(leftPos, leftV);

                verts.emplace_back(topVert);
                verts.emplace_back(rightVert);
                verts.emplace_back(bottomVert);
                verts.emplace_back(topVert);
                verts.emplace_back(bottomVert);
                verts.emplace_back(leftVert);
            }
        }
    });

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

    const int ox = tex->offsetX();
    const int oy = tex->offsetY();

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
            tl.position.x = x + s*vPosW + ox;
            tl.position.y = y + oy;
            tl.tex_coord.x = s*vTexCoordW;
            tl.tex_coord.y = 0.f;
            sprite.mapCoords(tl.tex_coord);
        }
        {
            indices.emplace_back(verts.size());
            auto& tr = verts.emplace_back();
            const float l = lightness[s + 1];
            tr.color = SDL_FColor{l, l, l, 1.f};
            tr.position.x = x + (s + 1)*vPosW + ox;
            tr.position.y = y + oy;
            tr.tex_coord.x = (s + 1)*vTexCoordW;
            tr.tex_coord.y = 0.f;
            sprite.mapCoords(tr.tex_coord);
        }
        {
            indices.emplace_back(verts.size());
            auto& br = verts.emplace_back();
            const float l = lightness[s + 1];
            br.color = SDL_FColor{l, l, l, 1.f};
            br.position.x = x + (s + 1)*vPosW + ox;
            br.position.y = y + th + oy;
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
            bl.position.x = x + s*vPosW + ox;
            bl.position.y = y + th + oy;
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
    const float singleShift = 1.f/sTileDiv;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    for(const auto& c : mRenderCalls) {
        const auto& cref = *c;
        const auto type = c->fType;
        std::vector<float> lightness;
        const int ictx = cref.fTX;
        const int icty = cref.fTY;

        switch(type) {
        case eRenderCallType::object: {
            float l = 0.f;
            const int x0 = (cref.fTX - ictx)/singleShift;
            const int y0 = (cref.fTY - icty)/singleShift;
            const auto handle = [&](const int dx, const int dy) {
                int x = x0 + dx;
                int y = y0 + dy;
                int tx = ictx;
                int ty = icty;
                if(x < 0) {
                    tx--;
                    x = sTileDiv + x;
                } else if(x > sTileDiv) {
                    tx++;
                    x -= sTileDiv + 1;
                }
                if(y < 0) {
                    ty--;
                    y = sTileDiv + y;
                } else if(y > sTileDiv) {
                    ty++;
                    y -= sTileDiv + 1;
                }
                const auto tile = mIterator.getTile(tx, ty);
                if(tile) {
                    const auto& lighting = tile->fLighting;
                    l += lighting[y*sNDots + x];
                }
            };

            handle(0, 0);
            handle(-1, 0);
            handle(0, -1);

            l = std::max(mLightness, l*0.33f);

            lightness.emplace_back(l);
            lightness.emplace_back(l);
        } break;
        case eRenderCallType::wall: {
            const auto tile = mIterator.getTile(ictx, icty);
            if(!tile) {
                lightness.emplace_back(mLightness);
                lightness.emplace_back(mLightness);
            } else {
                const auto handle = [&](const int dx, const int dy,
                                        const eWallType type) {
                    float l1 = mLightness;
                    float l2 = mLightness;

                    int ddx = 0;
                    int ddy = 0;
                    const auto t1 = tile;
                    const eTileInfo* t2 = nullptr;
                    switch(type) {
                    case eWallType::topLeft: {
                        t2 = mIterator.getTile(ictx - 1, icty);
                        ddx = sTileDiv - 2;
                    } break;
                    case eWallType::topRight: {
                        t2 = mIterator.getTile(ictx, icty - 1);
                        ddy = sTileDiv - 2;
                    } break;
                    }

                    l1 = t1->fLighting[dy*sNDots + dx];
                    if(t2) l2 = t2->fLighting[(dy + ddy)*sNDots + dx + ddx];

                    lightness.emplace_back(std::max(l1, l2));
                };

                const auto wtype = cref.fWallType;
                switch(wtype) {
                case eWallType::topLeft: {
                    for(int dy = sTileDiv; dy >= 0; dy--) {
                        handle(1, dy, wtype);
                    }
                } break;
                case eWallType::topRight: {
                    for(int dx = 0; dx <= sTileDiv; dx++) {
                        handle(dx, 1, wtype);
                    }
                } break;
                }
            }
        } break;
        }
        render(r, cref.fX, cref.fY, cref.fTex, lightness);
    }
}

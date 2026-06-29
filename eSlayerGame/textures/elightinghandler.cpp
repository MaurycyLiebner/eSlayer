#include "elightinghandler.h"

#include "../erendersettings.h"

#include <eSlayerHelpers/ethreads.h>
#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/epoint.h>

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <cmath>

eLightingHandler::eLightingHandler(
    eTilesIterator& tileIterator) :
    mIterator(tileIterator) {
    if(eThreads::sThreads < 0) {
        mWorkerCount = std::min(16u, std::thread::hardware_concurrency());
    } else {
        mWorkerCount = eThreads::sThreads;
    }
    mDivision = mWorkerCount + 1;
    for(int i = 0; i < mWorkerCount; i++) {
        mThreads.emplace_back([this, i] {
            workerLoop(i + 1);
        });
    }
}

eLightingHandler::~eLightingHandler() {
    if(mWorkerCount > 0) {
        {
            std::lock_guard lock(mMutex);
            mStop = true;
            mGeneration++;
        }

        mWakeup.notify_all();

        for(auto& t : mThreads) {
            t.join();
        }
    }
}

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

void eLightingHandler::addLight(const eLight& light) {
    mIterator.addLight(light);
}

void eLightingHandler::addBlocker(std::unique_ptr<eBlockerBase>& b) {
    mIterator.addBlocker(b);
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
    mTileDiv = eRenderSettings::sLightingQuality.fSubdivision;
    mNDots = mTileDiv + 1;

    const int c = mIterator.tileCount();
    mVerts.resize(4*c*mTileDiv*mTileDiv);
    mIndices.resize(6*c*mTileDiv*mTileDiv);

    mIterator.iterateOverCells([&](eCell& cell) {
        for(auto& l : cell.fLights) {
            const int minCellX = l.fMinCellX;
            const int maxCellX = l.fMaxCellX;
            const int minCellY = l.fMinCellY;
            const int maxCellY = l.fMaxCellY;

            for(int cellX = minCellX; cellX <= maxCellX; cellX++) {
                for(int cellY = minCellY; cellY <= maxCellY; cellY++) {
                    const auto cell = mIterator.getCellAtCellPos(cellX, cellY);
                    if(!cell) continue;
                    for(const auto& b : cell->fBlockers) {
                        l.fBlockers.emplace(b.get());
                    }
                }
            }
        }
    });

    if(mWorkerCount > 0) {
        {
            std::lock_guard lock(mMutex);

            mRemaining = mDivision;
            ++mGeneration;
        }

        mWakeup.notify_all();
    }

    // main thread participates with shift == 0
    calculate(0, mDivision);

    if(mWorkerCount > 0) {
        if(mRemaining.fetch_sub(1) == 1) {
            std::lock_guard lock(mMutex);
            mFinished.notify_one();
        }

        std::unique_lock lock(mMutex);
        mFinished.wait(lock, [this] { return mRemaining.load() == 0; });
    }
}

void eLightingHandler::renderFloorLighting(SDL_Renderer * const r) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_MUL);
    SDL_RenderGeometry(r, nullptr,
                       mVerts.data(), mVerts.size(),
                       mIndices.data(), mIndices.size());
}

void render(SDL_Renderer* const r,
            const float x, const float y,
            eTexture& tex,
            std::vector<float>& lightness,
            const SDL_FColor& colorMod,
            const float opacity,
            const float scale) {
    if(lightness.empty()) {
        lightness.emplace_back(0.f);
        lightness.emplace_back(0.f);
    } else if(lightness.size() == 1) {
        const float l = lightness[0];
        lightness.emplace_back(l);
    }
    const int nStrips = lightness.size() - 1;
    if(nStrips < 1) return;
    const float vTexCoordW = 1.f/nStrips;
    const float tw = tex.width()*scale;
    const float th = tex.height()*scale;
    const float vPosW = vTexCoordW*tw;

    const float ox = tex.offsetX()*scale;
    const float oy = tex.offsetY()*scale;

    std::vector<SDL_Vertex> verts;
    const int nVerts = 2*(nStrips + 1);
    verts.reserve(nVerts);

    std::vector<int> indices;
    const int nIndices = 6*nStrips;
    indices.reserve(nIndices);

    const auto sprite = tex.sprite();
    const float alpha = colorMod.a*opacity;
    for(int i = 0; i <= nStrips; ++i) {
        if(i != nStrips) {
            const int tl = 2 * i;
            const int bl = tl + 1;
            const int tr = tl + 2;
            const int br = tl + 3;

            indices.emplace_back(tl);
            indices.emplace_back(tr);
            indices.emplace_back(br);

            indices.emplace_back(tl);
            indices.emplace_back(br);
            indices.emplace_back(bl);
        }

        const float l = lightness[i];

        const SDL_FColor color{
            l * colorMod.r,
            l * colorMod.g,
            l * colorMod.b,
            alpha
        };

        const float px = x + i * vPosW + ox;
        const float tx = i * vTexCoordW;

        // top
        {
            auto& v = verts.emplace_back();
            v.color = color;
            v.position = {px, y + oy};
            v.tex_coord = {tx, 0.f};
            sprite.mapCoords(v.tex_coord);
        }

        // bottom
        {
            auto& v = verts.emplace_back();
            v.color = color;
            v.position = {px, y + th + oy};
            v.tex_coord = {tx, 1.f};
            sprite.mapCoords(v.tex_coord);
        }
    }

    const auto sdlTex = sprite.fTex;
    SDL_RenderGeometry(r, sdlTex,
                       verts.data(), verts.size(),
                       indices.data(), indices.size());
}

void drawShadow(
    SDL_Renderer* const r,
    const int drawX,
    const int drawY,
    const eTexture& tex,
    const float skew,
    const float scaleY,
    const float lightness,
    const float alpha) {
    const float w = tex.width();
    const float h = tex.height() * scaleY;

    const float skewOffset = h * skew;

    const float x = drawX - skewOffset;
    const float y = drawY - h;

    SDL_Vertex verts[4];

    verts[0].position = { x, y };
    verts[1].position = { x + w, y };
    verts[2].position = { x + w + skewOffset, y + h };
    verts[3].position = { x + skewOffset, y + h };

    for(auto& v : verts) {
        v.color = { lightness, lightness, lightness, alpha };
    }

    const auto sprite = tex.sprite();

    verts[0].tex_coord = { sprite.fTexCoordLeft, sprite.fTexCoordTop };
    verts[1].tex_coord = { sprite.fTexCoordRight, sprite.fTexCoordTop };
    verts[2].tex_coord = { sprite.fTexCoordRight, sprite.fTexCoordBottom };
    verts[3].tex_coord = { sprite.fTexCoordLeft, sprite.fTexCoordBottom };

    static constexpr int indices[6] = { 0, 1, 2, 0, 2, 3 };

    const auto sdlTex = sprite.fTex;
    SDL_RenderGeometry(r, sdlTex, verts, 4, indices, 6);
}

void drawShadow(
    SDL_Renderer* const r,
    const int drawX,
    const int drawY,
    const eTexture& tex) {
    const float skew = 0.5f;
    const float scaleY = 0.5f;
    const float lightness = 0.f;
    const float alpha = 0.5f;
    ::drawShadow(r, drawX, drawY, tex, skew, scaleY,
                 lightness, alpha);
}

void eLightingHandler::render(
    SDL_Renderer* const r,
    const eRenderCall& c) const {
    const float singleShift = 1.f/mTileDiv;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    const auto type = c.fType;
    std::vector<float> lightness;
    const int ictx = c.fTX;
    const int icty = c.fTY;

    if(c.fShadow && c.fTex) {
        const auto& tex = *c.fTex;
        const int texH = tex.height();
        drawShadow(r, c.fX, c.fY + texH, tex);
    }

    const float addL = c.fHighlight ? 0.5f : 0.f;
    if(c.fLighting) {
        lightness.emplace_back(1.f + addL);
    } else {
        const int x0 = (c.fTX - ictx)/singleShift;
        const int y0 = (c.fTY - icty)/singleShift;
        switch(type) {
        case eRenderCallType::object: {
            if(c.fObjSplitLighting) {
                const auto handle = [&](const int dx, const int dy) {
                    int x = x0 + dx;
                    int y = y0 + dy;
                    int tx = ictx;
                    int ty = icty;
                    const int tileSize = mTileDiv + 1;
                    if(x < 0) {
                        const int d = (-x + tileSize - 1) / tileSize;
                        tx -= d;
                        x += d * tileSize;
                    } else if (x > mTileDiv) {
                        const int d = x / tileSize;
                        tx += d;
                        x -= d * tileSize;
                    }

                    if(y < 0) {
                        const int d = (-y + tileSize - 1) / tileSize;
                        ty -= d;
                        y += d * tileSize;
                    } else if (y > mTileDiv) {
                        const int d = y / tileSize;
                        ty += d;
                        y -= d * tileSize;
                    }
                    const auto tile = mIterator.getTile(tx, ty);
                    if(tile) {
                        const auto& lighting = tile->fLighting;
                        const float l = lighting[y*mNDots + x] + addL;
                        lightness.emplace_back(l);
                    } else {
                        lightness.emplace_back(mLightness + addL);
                    }
                };

                const auto& tex = *c.fTex;
                const int tox = tex.offsetX();
                const int tw = tex.width();
                const float left = tox + 0.5f*(mBaseTileW - tw);
                const float xMin = 2.f*left/mBaseTileW;
                const int dxMin = -mTileDiv*(c.fObjSize - xMin);
                const float right = left + tw;
                const float xMax = 2 - 2.f*right/mBaseTileW;
                const int dyMin = -mTileDiv*(c.fObjSize - xMax);

                for(int dx = dxMin; dx <= 0; dx++) {
                    handle(dx, 0);
                }
                for(int dy = 0; dy >= dyMin; dy--) {
                    handle(0, dy);
                }
            } else {
                float l = 0.f;
                const auto handle = [&](const int dx, const int dy) {
                    int x = x0 + dx;
                    int y = y0 + dy;
                    int tx = ictx;
                    int ty = icty;
                    if(x < 0) {
                        tx--;
                        x += mTileDiv;
                    } else if(x > mTileDiv) {
                        tx++;
                        x -= mTileDiv + 1;
                    }
                    if(y < 0) {
                        ty--;
                        y += mTileDiv;
                    } else if(y > mTileDiv) {
                        ty++;
                        y -= mTileDiv + 1;
                    }
                    const auto tile = mIterator.getTile(tx, ty);
                    if(tile) {
                        const auto& lighting = tile->fLighting;
                        l += lighting[y*mNDots + x];
                    }
                };

                handle(0, 0);
                handle(-1, 0);
                handle(0, -1);

                l = std::max(mLightness, l*0.33f) + addL;

                lightness.emplace_back(l);
            }
        } break;
        case eRenderCallType::area:
        case eRenderCallType::missile:
        case eRenderCallType::unit:
        case eRenderCallType::item: {
            float l = addL;
            const auto tile = mIterator.getTile(ictx, icty);
            if(tile) {
                const auto& lighting = tile->fLighting;
                l = lighting[y0*mNDots + x0] + addL;
            }
            lightness.emplace_back(l);
        } break;
        case eRenderCallType::wall: {
            const auto tile = mIterator.getTile(ictx, icty);
            if(!tile) {
                lightness.emplace_back(mLightness + addL);
                lightness.emplace_back(mLightness + addL);
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
                        ddx = mTileDiv - 2;
                    } break;
                    case eWallType::topRight: {
                        t2 = mIterator.getTile(ictx, icty - 1);
                        ddy = mTileDiv - 2;
                    } break;
                    }

                    l1 = t1->fLighting[dy*mNDots + dx];
                    if(t2) l2 = t2->fLighting[(dy + ddy)*mNDots + dx + ddx];

                    lightness.emplace_back(std::max(l1, l2) + addL);
                };

                const auto wtype = c.fWallType;
                switch(wtype) {
                case eWallType::topLeft: {
                    for(int dy = mTileDiv; dy >= 0; dy--) {
                        handle(1, dy, wtype);
                    }
                } break;
                case eWallType::topRight: {
                    for(int dx = 0; dx <= mTileDiv; dx++) {
                        handle(dx, 1, wtype);
                    }
                } break;
                }
            }
        } break;
        }
    }
    const float opacity = c.fTransparent ? 0.5f : 1.f;
    ::render(r, c.fX, c.fY, *c.fTex, lightness, c.fColorMod, opacity, c.fScale);
}

void eLightingHandler::workerLoop(
    const int shift) {
    int localGeneration = 0;

    while(true) {
        int generation;

        {
            std::unique_lock lock(mMutex);

            mWakeup.wait(lock, [&] {
                return mStop || mGeneration != localGeneration;
            });

            if(mStop) return;

            generation = mGeneration;
        }

        calculate(shift, mDivision);

        if(mRemaining.fetch_sub(1) == 1) {
            std::lock_guard lock(mMutex);
            mFinished.notify_one();
        }

        localGeneration = generation;
    }
}

void eLightingHandler::calculate(
    const int shift, const int division) {

    const auto make = [](const ePointF& p, const float i) {
        SDL_Vertex v;
        v.position = {p.fX, p.fY};
        v.color = SDL_FColor{i, i, i, 1.f};
        return v;
    };

    const int halfW = mBaseTileW/2;
    const int halfH = (mBaseTileH + 1)/2;

    const auto shiftP = [&](ePointF& p,
                            const float dtx,
                            const float dty) {
        p.fX += (dtx - dty)*halfW;
        p.fY += (dtx + dty)*halfH;
    };

    const float singleShift = 1.f/mTileDiv;
    mIterator.iterate(shift, division, [&](eTileInfo& tile) {
        const auto lightCell = mIterator.getCellAtPos(
            tile.fTX, tile.fTY);
        auto& lighting = tile.fLighting;
        if(!lightCell) {
            lighting.resize(mNDots*mNDots, mLightness);
        } else {
            const auto& lights = lightCell->fLights;
            lighting.resize(mNDots*mNDots);
            for(int y = 0; y < mNDots; y++) {
                const int yShift = y*mNDots;
                const float ty = tile.fTY + float(y)/mTileDiv;
                for(int x = 0; x < mNDots; x++) {
                    const float tx = tile.fTX + float(x)/mTileDiv;
                    float& v = lighting[yShift + x];
                    v = mLightness;
                    const ePointF tp{tx, ty};
                    for(const auto& l : lights) {
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
                        const auto& blockers = l.fBlockers;
                        for(const auto& b : blockers) {
                            const auto& bref = *b;
                            switch(bref.fType) {
                            case eBlockerBaseType::object: {
                                const auto& oref = static_cast<const eObjectLightBlocker&>(bref);
                                const float s = 0.5f*oref.fSize;
                                const ePointF oc{bref.fTX + s, bref.fTY + s};
                                const ePointF o1 = oc + perp*s;
                                const ePointF o2 = oc - perp*s;
                                ePointF inters;
                                const bool r = lineIntersection(tp, lp, o1, o2, &inters);
                                if(r) {
                                    const float dist = ePointF::distance(oc, inters);
                                    mult = std::min(mult, 1.f - std::clamp((s - dist)/mFeatherLen, 0.f, 1.f));
                                }
                            } break;
                            case eBlockerBaseType::rect: {
                                const auto& oref = static_cast<const eRectLightBlocker&>(bref);
                                const ePointF pt{bref.fTX, bref.fTY};
                                const ePointF pr{bref.fTX + oref.fWidth, bref.fTY};
                                const ePointF pb{bref.fTX + oref.fWidth, bref.fTY + oref.fHeight};
                                const ePointF pl{bref.fTX, bref.fTY + oref.fHeight};
                                const auto handlePts = [&](const ePointF& p1, const ePointF& p2) {
                                    ePointF inters;
                                    const bool r = lineIntersection(tp, lp, p1, p2, &inters);
                                    if(r) {
                                        const float dist1 = ePointF::distance(p1, inters);
                                        const float dist2 = ePointF::distance(p2, inters);
                                        const float dist = std::min(dist1, dist2);
                                        mult = std::min(mult, 1.f - std::clamp(dist/mFeatherLen, 0.f, 1.f));
                                    }
                                };
                                handlePts(pt, pr);
                                handlePts(pr, pb);
                                handlePts(pb, pl);
                                handlePts(pl, pt);
                            } break;
                            case eBlockerBaseType::wall: {
                                const auto& wref = static_cast<const eWallLightBlocker&>(bref);
                                const int itx = wref.fITX;
                                const int ity = wref.fITY;
                                const auto& p1 = wref.fP1;
                                const auto& p2 = wref.fP2;
                                const bool featherMin = wref.fMinFeather;
                                const bool featherMax = wref.fMaxFeather;
                                switch(wref.fDir) {
                                case eWallType::topLeft: {
                                    if(tx == itx && tile.fTY != ity) continue;
                                    if((tile.fTX >= itx) == (l.fTX >= itx)) {
                                        continue;
                                    }
                                } break;
                                case eWallType::topRight: {
                                    if(ty == ity && tile.fTX != itx) continue;
                                    if((tile.fTY >= ity) == (l.fTY >= ity)) {
                                        continue;
                                    }
                                } break;
                                }
                                ePointF inters;
                                const bool r = lineIntersection(tp, lp, p1, p2, &inters);
                                if(r) {
                                    if(!featherMin && !featherMax) {
                                        mult = 0.f;
                                    } else {
                                        if(featherMin) {
                                            const float dist = ePointF::distance(p1, inters);
                                            const float t = std::clamp(dist/mFeatherLen, 0.f, 1.f);
                                            mult = std::min(mult, 1.f - t);
                                        }
                                        if(featherMax) {
                                            const float dist = ePointF::distance(p2, inters);
                                            const float t = std::clamp(dist/mFeatherLen, 0.f, 1.f);
                                            mult = std::min(mult, 1.f - t);
                                        }
                                    }
                                }
                            } break;
                            }
                            if(mult <= 0.f) break;
                        }
                        if(mult <= 0.f) continue;

                        const float t = dist/l.fRadius;
                        const float i = 1.f - t*t*t*t;
                        v = std::max(v, i*mult);
                    }
                }
            }
        }

        int vid = 4*mTileDiv*mTileDiv*tile.fId;
        int iid = 6*mTileDiv*mTileDiv*tile.fId;
        for(int y = 0; y < mTileDiv; y++) {
            const float dty = float(y)/mTileDiv;
            const float ty = tile.fTY + dty;
            for(int x = 0; x < mTileDiv; x++) {
                const float dtx = float(x)/mTileDiv;
                const float tx = tile.fTX + dtx;

                const float topV = lighting[y*mNDots + x];
                const float rightV = lighting[y*mNDots + x + 1];
                const float bottomV = lighting[(y + 1)*mNDots + x + 1];
                const float leftV = lighting[(y + 1)*mNDots + x];

                ePointF topPos{tile.fPX, tile.fPY};
                shiftP(topPos, dtx, dty);
                const auto topVert = make(topPos, topV);

                ePointF rightPos{tile.fPX, tile.fPY};
                shiftP(rightPos, dtx + singleShift, dty);
                const auto rightVert = make(rightPos, rightV);

                ePointF bottomPos{tile.fPX, tile.fPY};
                shiftP(bottomPos, dtx + singleShift, dty + singleShift);
                const auto bottomVert = make(bottomPos, bottomV);

                ePointF leftPos{tile.fPX, tile.fPY};
                shiftP(leftPos, dtx, dty + singleShift);
                const auto leftVert = make(leftPos, leftV);

                const int topVID = vid++;
                const int rightVID = vid++;
                const int bottomVID = vid++;
                const int leftVID = vid++;

                mIndices[iid++] = topVID;
                mIndices[iid++] = rightVID;
                mIndices[iid++] = bottomVID;
                mIndices[iid++] = topVID;
                mIndices[iid++] = bottomVID;
                mIndices[iid++] = leftVID;

                mVerts[topVID] = topVert;
                mVerts[rightVID] = rightVert;
                mVerts[bottomVID] = bottomVert;
                mVerts[leftVID] = leftVert;
            }
        }
    });
}

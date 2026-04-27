#include "elightingtexture.h"

#include "eeffectstextures.h"
#include "../widgets/epainter.h"

#include <eSlayerHelpers/evec2.h>
#include <eSlayerHelpers/epoint.h>

#include <cmath>

void eLightingTexture::initialize(SDL_Renderer * const r,
                                  const int w, const int h,
                                  const SDL_Color& color) {
    mColor = color;
    create(r, w, h, color);
    setBlendMode(SDL_BLENDMODE_MOD);

    const int id = eEffectsTextures::sEffects.id("lighting");
    const auto& lighting = eEffectsTextures::sEffects.get(id);
    mLightingTex = lighting.getTexture(0);
    mLightingTex->setBlendMode(SDL_BLENDMODE_ADD);
}

void eLightingTexture::setClearColor(const SDL_Color& color) {
    mColor = color;
}

void eLightingTexture::clear(SDL_Renderer * const r) {
    fill(r, mColor);
}

void eLightingTexture::renderLight(
    const eResolution& res,
    SDL_Renderer * const r,
    const eLight& light,
    const std::vector<eLightBlocker>& blockers) {
    const float mult = res.multiplier();
    const float radius = light.fRadius;
    const auto& color = light.fColor;
    const float x = light.fPX;
    const float y = light.fPY;
    const float scale = radius/10.f;
    const float srcW = mLightingTex->width();
    const float srcH = mLightingTex->height();
    const float dstW = scale*srcW;
    const float dstH = scale*srcH;
    const SDL_FRect srcRect{0.f, 0.f, srcW, srcH};
    const float dstX = x - dstW/2.f;
    const float dstY = y - dstH/2.f;
    const SDL_FRect dstRect{dstX, dstY, dstW, dstH};
    const auto lightTex = std::make_shared<eTexture>();
    lightTex->create(r, dstW, dstH, SDL_Color{0, 0, 0, 255});
    lightTex->setBlendMode(SDL_BLENDMODE_ADD);
    ePainter p(r);
    {
        const SDL_FRect tmpDstRect{0.f, 0.f, dstW, dstH};
        const auto h = lightTex->createTargetHolder(r);
        mLightingTex->setColorMod(color.r, color.g, color.b);
        mLightingTex->setAlpha(color.a);
        mLightingTex->render(r, srcRect, tmpDstRect);

        const auto shadowTex = std::make_shared<eTexture>();
        shadowTex->create(r, dstW, dstH, SDL_Color{255, 255, 255, 255});
        shadowTex->setBlendMode(SDL_BLENDMODE_MUL);

        {
            std::map<float, eLightBlocker> aboveBlockers;
            std::map<float, eLightBlocker> belowBlockers;
            const auto h = shadowTex->createTargetHolder(r);
            for(const auto& b : blockers) {
                const auto& bTex = b.fTex;
                const float bTexW = bTex->width();
                const float bTexH = bTex->height();
                const SDL_FRect bTexRect{b.fPX - 0.5f*bTexW,
                                         b.fPY - bTexH,
                                         bTexW, bTexH};
                if(!SDL_HasRectIntersectionFloat(&bTexRect, &dstRect)) continue;
                const float dx = x - b.fPX;
                const float dy = y - b.fTileCenterY;
                const float key = -(dx*dx + dy*dy);
                if(dy > 0) {
                    aboveBlockers.emplace(key, b);
                } else {
                    belowBlockers.emplace(key, b);
                }
            }

            const auto handleBlocker = [&](const eLightBlocker& b) {
                const auto& bTex = b.fTex;
                const float bTexW = bTex->width();
                const ePointF objPt{b.fPX - dstX, b.fTileCenterY - dstY};
                const ePointF lightPt{x - dstX, y - dstY};

                // Direction from light to object
                eVec2f dir = ePointF::vector(objPt, lightPt);
                const float len = dir.length();
                if(len < 0.001f) return;
                dir /= len; // normalize

                // Project both points away from light
                const float shadowLen = dstW; // or radius * 2

                // Build perpendicular as before
                const eVec2f perp{-dir.y, dir.x};

                const float isoScaleX = 1.f;
                const float isoScaleY = 0.2f; // typical isometric vertical squash

                // project perp into iso space
                const float projectedScale =
                    std::sqrt((perp.x * perp.x) * isoScaleX +
                              (perp.y * perp.y) * isoScaleY);

                const float halfW = mult * b.fSize * 33.3f * projectedScale;

                // Edge points
                const ePointF leftPt  = objPt + perp * halfW;
                const ePointF rightPt = objPt - perp * halfW;

                renderShadow(res, r, lightPt, leftPt, rightPt,
                             true, true, true, shadowLen);

                const float dy = y - b.fTileCenterY;
                const float lightness = std::clamp(0.05f*dy/mult, 0.f, 255.f);
                p.drawShadow(b.fPX - 0.5f*bTexW - dstX,
                             b.fPY - dstY, *bTex,
                             0.f, 1.f, lightness, 1.f);
            };

            for(const auto& it : aboveBlockers) {
                const auto& b = it.second;
                handleBlocker(b);
            }

            const auto& paintCall = light.fPaintCall;
            if(paintCall.fTex) {
                const auto& tex = *paintCall.fTex;
                p.drawShadow(paintCall.fX - dstX,
                             paintCall.fY - dstY + tex.height(),
                             tex, 0.f, 1.f, 255.f, 1.f);
            }

            for(const auto& it : belowBlockers) {
                const auto& b = it.second;
                handleBlocker(b);
            }
        }

        shadowTex->render(r, 0, 0);
    }
    {
        const auto h = createTargetHolder(r);
        lightTex->render(r, dstX, dstY);
    }
}

void eLightingTexture::renderShadow(
    const eResolution& res,
    SDL_Renderer * const r,
    const ePointF& lightPt,
    const ePointF& leftPt,
    const ePointF& rightPt,
    const bool rightFeather,
    const bool leftFeather,
    const bool nearFeather,
    const float shadowLen) const {
    const float mult = res.multiplier();
    const float softness = mult*20.f;
    // Direction from light to each edge point
    eVec2f dirLeft  = ePointF::vector(leftPt, lightPt);
    eVec2f dirRight = ePointF::vector(rightPt, lightPt);

    if(dirLeft.length() < 0.001f) return;
    if(dirRight.length() < 0.001f) return;

    dirLeft.normalize(1.f);
    dirRight.normalize(1.f);

    // Far projected points
    const ePointF farLeft  = leftPt  + dirLeft  * shadowLen;
    const ePointF farRight = rightPt + dirRight * shadowLen;

    // Approximate "sideways" direction (not assuming symmetry)
    eVec2f edgeDir = ePointF::vector(leftPt, rightPt);
    if(edgeDir.length() < 0.001f) return;

    edgeDir.normalize(1.f);
    eVec2f perp{-edgeDir.y, edgeDir.x};

    // Ensure perp points away from the light
    const auto middle = rightPt + ePointF::vector(leftPt, rightPt)*0.5f;
    const eVec2f toLight = ePointF::vector(lightPt, middle);

    if(eVec2f::dot(perp, toLight) < 0.f) {
        perp = {-perp.x, -perp.y};
    }

    // Near softness (towards light)
    const ePointF nearLeft  = leftPt  - dirLeft  * softness;
    const ePointF nearRight = rightPt - dirRight * softness;

    // Side expansion
    const ePointF leftOuter     = leftPt  + perp * softness;
    const ePointF rightOuter    = rightPt + perp * softness;

    const ePointF farLeftOuter  = farLeft  + perp * softness + dirLeft  * softness;
    const ePointF farRightOuter = farRight + perp * softness + dirRight * softness;

    std::vector<SDL_Vertex> verts;

    auto make = [](const ePointF& p, float a) {
        SDL_Vertex v;
        v.position = {p.fX, p.fY};
        v.color = SDL_FColor{0.f, 0.f, 0.f, a};
        v.tex_coord = {0, 0};
        return v;
    };

    const float coreA = 1.f;
    const float edgeA = 0.f;

    // ---- CORE
    verts.push_back(make(leftPt, coreA));
    verts.push_back(make(rightPt, coreA));
    verts.push_back(make(farRight, coreA));

    verts.push_back(make(leftPt, coreA));
    verts.push_back(make(farRight, coreA));
    verts.push_back(make(farLeft, coreA));

    // ---- NEAR EDGE FEATHER
    if(nearFeather) {
        verts.push_back(make(nearLeft, edgeA));
        verts.push_back(make(nearRight, edgeA));
        verts.push_back(make(rightPt, coreA));

        verts.push_back(make(nearLeft, edgeA));
        verts.push_back(make(rightPt, coreA));
        verts.push_back(make(leftPt, coreA));
    }

    // ---- LEFT SIDE FEATHER
    if(leftFeather) {
        verts.push_back(make(leftPt, coreA));
        verts.push_back(make(leftOuter, edgeA));
        verts.push_back(make(farLeftOuter, edgeA));

        verts.push_back(make(leftPt, coreA));
        verts.push_back(make(farLeftOuter, edgeA));
        verts.push_back(make(farLeft, coreA));
    }

    // ---- RIGHT SIDE FEATHER
    if(rightFeather) {
        verts.push_back(make(rightPt, coreA));
        verts.push_back(make(farRight, coreA));
        verts.push_back(make(farRightOuter, edgeA));

        verts.push_back(make(rightPt, coreA));
        verts.push_back(make(farRightOuter, edgeA));
        verts.push_back(make(rightOuter, edgeA));
    }

    // ---- NEAR CORNERS
    if(nearFeather && leftFeather) {
        verts.push_back(make(nearLeft, edgeA));
        verts.push_back(make(leftOuter, edgeA));
        verts.push_back(make(leftPt, coreA));
    }

    if(nearFeather && rightFeather) {
        verts.push_back(make(nearRight, edgeA));
        verts.push_back(make(rightPt, coreA));
        verts.push_back(make(rightOuter, edgeA));
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_RenderGeometry(r, nullptr, verts.data(),
                       verts.size(), nullptr, 0);
}

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
                const float key = dx*dx + dy*dy;
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

                // Use REAL object width (no scaling)
                const float halfW = mult*b.fSize*33.3f;

                       // Build perpendicular as before
                const eVec2f perp{-dir.y, dir.x};

                // Edge points
                const ePointF leftPt  = objPt + perp * halfW;
                const ePointF rightPt = objPt - perp * halfW;

                // Now project EACH edge independently (this is the key!)
                auto dirLeft  = ePointF::vector(leftPt, lightPt);
                auto dirRight = ePointF::vector(rightPt, lightPt);

                dirLeft.normalize(1.f);
                dirRight.normalize(1.f);

                const ePointF farLeft  = leftPt  + dirLeft  * shadowLen;
                const ePointF farRight = rightPt + dirRight * shadowLen;

                // Build quad as two triangles
                SDL_Vertex verts[6];

                // Triangle 1
                verts[0].position = {leftPt.fX, leftPt.fY};
                verts[1].position = {rightPt.fX, rightPt.fY};
                verts[2].position = {farRight.fX, farRight.fY};

                // Triangle 2
                verts[3].position = {leftPt.fX, leftPt.fY};
                verts[4].position = {farRight.fX, farRight.fY};
                verts[5].position = {farLeft.fX, farLeft.fY};

                // All black
                for(int i = 0; i < 6; i++) {
                    verts[i].color = SDL_FColor{0, 0, 0, 1.f};
                    verts[i].tex_coord = SDL_FPoint{0, 0};
                }

                // Blend: darken
                SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

                // Render geometry
                SDL_RenderGeometry(r, nullptr, verts, 6, nullptr, 0);

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
                p.drawShadow(paintCall.fX, paintCall.fY,
                             *paintCall.fTex,
                             0.f, 1.f, 255.f, 1.f);
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

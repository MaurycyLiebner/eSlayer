#include "elightingtexture.h"

#include "eeffectstextures.h"

void eLightingTexture::initialize(SDL_Renderer * const r,
                                  const int w, const int h,
                                  const SDL_Color& color) {
    mColor = color;
    create(r, w, h, color);
    setBlendMode(SDL_BLENDMODE_MOD);

    const auto lighting = eEffectsTextures::get("lighting");
    mLightingTex = lighting->getTexture(0);
    mLightingTex->setBlendMode(SDL_BLENDMODE_ADD);
}

void eLightingTexture::setClearColor(const SDL_Color& color) {
    mColor = color;
}

void eLightingTexture::clear(SDL_Renderer * const r) {
    fill(r, mColor);
}

void eLightingTexture::renderLight(
    SDL_Renderer * const r,
    const float x, const float y,
    const float radius,
    const SDL_Color& color) {
    const float scale = radius/10;
    const float srcW = mLightingTex->width();
    const float srcH = mLightingTex->height();
    const float dstW = scale*srcW;
    const float dstH = scale*srcH;
    const SDL_FRect dstRect{x - dstW/2,
                            y - dstH/2,
                            dstW, dstH};
    const SDL_FRect srcRect{0.f, 0.f, srcW, srcH};
    mLightingTex->setColorMod(color.r, color.g, color.b);
    mLightingTex->setAlpha(color.a);
    mLightingTex->render(r, srcRect, dstRect);
}

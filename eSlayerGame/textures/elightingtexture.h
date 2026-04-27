#ifndef ELIGHTINGTEXTURE_H
#define ELIGHTINGTEXTURE_H

#include "etexture.h"
#include "epaintcall.h"

#include <vector>

class eResolution;

struct eLight {
    eLight(const float px,
           const float py,
           const float radius,
           const SDL_Color& color,
           const ePaintCall& paintCall) :
        fPX(px), fPY(py),
        fRadius(radius),
        fColor(color),
        fPaintCall(paintCall) {}

    float fPX;
    float fPY;
    float fRadius;
    SDL_Color fColor;
    ePaintCall fPaintCall;
};

struct eLightBlocker {
    eLightBlocker(const float px,
                  const float py,
                  const float cy,
                  const float size,
                  const std::shared_ptr<eTexture>& tex) :
        fPX(px), fPY(py),
        fTileCenterY(cy),
        fSize(size),
        fTex(tex) {}
    float fPX;
    float fPY;
    float fTileCenterY;
    float fSize;
    std::shared_ptr<eTexture> fTex;
};

class eLightingTexture : public eTexture {
public:
    using eTexture::eTexture;

    void initialize(SDL_Renderer* const r,
                    const int w, const int h,
                    const SDL_Color& color);
    void setClearColor(const SDL_Color& color);
    void clear(SDL_Renderer * const r);
    void renderLight(
        const eResolution& res,
        SDL_Renderer * const r,
        const eLight& light,
        const std::vector<eLightBlocker>& blockers);
private:
    SDL_Color mColor;
    std::shared_ptr<eTexture> mLightingTex;
};

#endif // ELIGHTINGTEXTURE_H

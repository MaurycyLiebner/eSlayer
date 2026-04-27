#ifndef ELIGHTINGTEXTURE_H
#define ELIGHTINGTEXTURE_H

#include "etexture.h"
#include "epaintcall.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/eterrstexturesdata.h>

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

struct eWallLightBlocker {
    eWallLightBlocker(const float px,
                      const float py,
                      const eBlockLightDirection dir,
                      const int tileW,
                      const int tileH,
                      const std::shared_ptr<eTexture>& tex) :
        fPX(px), fPY(py),
        fDir(dir),
        fTileW(tileW),
        fTileH(tileH),
        fTex(tex) {}
    float fPX;
    float fPY;
    eBlockLightDirection fDir;
    int fTileW;
    int fTileH;
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
        const std::vector<eLightBlocker>& blockers,
        const std::vector<eWallLightBlocker>& walls);
    void renderShadow(
        const eResolution& res,
        SDL_Renderer * const r,
        const ePointF& lightPt,
        const ePointF& leftPt,
        const ePointF& rightPt,
        const bool rightFeather,
        const bool leftFeather,
        const bool nearFeather,
        const float shadowLen) const;
private:
    SDL_Color mColor;
    std::shared_ptr<eTexture> mLightingTex;
};

#endif // ELIGHTINGTEXTURE_H

#ifndef ELIGHTINGTEXTURE_H
#define ELIGHTINGTEXTURE_H

#include "etexture.h"
#include "epaintcall.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/eterrstexturesdata.h>

#include <vector>

class eResolution;

struct eLight {
    eLight(const float tx,
           const float ty,
           const float px,
           const float py,
           const float radius,
           const SDL_Color& color,
           const ePaintCall& paintCall) :
        fTX(tx), fTY(ty),
        fPX(px), fPY(py),
        fRadius(radius),
        fColor(color),
        fPaintCall(paintCall) {}

    float fTX;
    float fTY;
    float fPX;
    float fPY;
    float fRadius;
    SDL_Color fColor;
    ePaintCall fPaintCall;
};

struct eBlockerBase {
    eBlockerBase(const float px,
                 const float py,
                 const std::shared_ptr<eTexture>& tex) :
        fPX(px), fPY(py),
        fTex(tex) {}

    float fPX;
    float fPY;
    std::shared_ptr<eTexture> fTex;
};

struct eLightBlocker : public eBlockerBase {
    eLightBlocker(const float px,
                  const float py,
                  const float cy,
                  const float size,
                  const std::shared_ptr<eTexture>& tex) :
        eBlockerBase(px, py, tex),
        fTileCenterY(cy),
        fSize(size) {}
    float fTileCenterY;
    float fSize;
};

struct eWallLightBlocker : public eBlockerBase {
    eWallLightBlocker(const int tx,
                      const int ty,
                      const float px,
                      const float py,
                      const eWallType dir,
                      const int tileW,
                      const int tileH,
                      const std::shared_ptr<eTexture>& tex) :
        eBlockerBase(px, py, tex),
        fTX(tx), fTY(ty),
        fDir(dir),
        fTileW(tileW),
        fTileH(tileH) {}
    int fTX;
    int fTY;
    eWallType fDir;
    int fTileW;
    int fTileH;
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
        SDL_Renderer * const r,
        const ePointF& lightPt,
        const ePointF& leftPt,
        const ePointF& rightPt,
        const bool rightFeather,
        const bool leftFeather,
        const bool nearFeather,
        const float shadowLen,
        const float softness) const;
private:
    SDL_Color mColor;
    std::shared_ptr<eTexture> mLightingTex;
};

#endif // ELIGHTINGTEXTURE_H

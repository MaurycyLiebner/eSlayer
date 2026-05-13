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
           const float radius) :
        fTX(tx), fTY(ty),
        fPX(px), fPY(py),
        fRadius(radius) {}

    float fTX;
    float fTY;
    float fPX;
    float fPY;
    float fRadius;
};

enum class eBlockerBaseType {
    object, wall
};

struct eBlockerBase {
    eBlockerBase(const eBlockerBaseType type,
                 const float px,
                 const float py,
                 const std::shared_ptr<eTexture>& tex) :
        fType(type),
        fPX(px), fPY(py),
        fTex(tex) {}

    eBlockerBaseType fType;
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
        eBlockerBase(eBlockerBaseType::object, px, py, tex),
        fTileCenterY(cy),
        fSize(size) {}
    float fTileCenterY;
    float fSize;
    float fLighting;
};

struct eWallLightBlocker : public eBlockerBase {
    eWallLightBlocker(const int tx,
                      const int ty,
                      const float px,
                      const float py,
                      const eWallType dir,
                      const int tileW,
                      const int tileH,
                      const std::shared_ptr<eTexture>& tex,
                      const float wallMin,
                      const float wallMax) :
        eBlockerBase(eBlockerBaseType::wall, px, py, tex),
        fTX(tx),
        fTY(ty),
        fDir(dir),
        fTileW(tileW),
        fTileH(tileH),
        fWallMin(wallMin),
        fWallMax(wallMax) {}
    int fTX;
    int fTY;
    eWallType fDir;
    int fTileW;
    int fTileH;
    float fWallMin;
    float fWallMax;
    const int mWallLightingW = 32;
    std::vector<float> fLighting;
};

class eLightingTexture : public eTexture {
public:
    using eTexture::eTexture;

    void initialize(SDL_Renderer* const r,
                    const int w, const int h,
                    const int tileW, const int tileH,
                    const SDL_Color& color);
    void setClearColor(const SDL_Color& color);
    void clear(SDL_Renderer * const r);

    void addLight(const eLight& light);
    void addBlocker(std::unique_ptr<eBlockerBase>& b);
    void calculateFloorLighting(const float tx0,
                                const float ty0);
    void renderFloorLighting(SDL_Renderer * const r);
private:
    SDL_Color mColor;
    std::vector<eLight> mLights;
    std::vector<std::unique_ptr<eBlockerBase>> mBlockers;
    const float sTileDimMult = 0.1f;
    const float sTileDimMultInv = 1.f/sTileDimMult;
    int mNCols;
    int mNRows;
    int mBaseTileW;
    int mBaseTileH;
    int mFloorLightW;
    int mFloorLightH;
    std::vector<std::vector<float>> mFloorLighting;
};

#endif // ELIGHTINGTEXTURE_H

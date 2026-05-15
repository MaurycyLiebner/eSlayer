#ifndef ELIGHTINGHANDLER_H
#define ELIGHTINGHANDLER_H

#include "epaintcall.h"

#include "etilesiterator.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/eterrstexturesdata.h>

#include <memory>
#include <vector>

class eResolution;
class SDL_Renderer;

struct eLight {
    eLight(const float tx,
           const float ty,
           const float radius) :
        fTX(tx), fTY(ty),
        fRadius(radius) {}

    float fTX;
    float fTY;
    float fRadius;
};

enum class eBlockerBaseType {
    object, wall
};

enum class eRenderCallType {
    unit, missile, object, wall
};

struct eRenderCall : public ePaintCall {
    eRenderCall(const eRenderCallType type,
                const float tx, const float ty,
                const float px, const float py,
                const std::shared_ptr<eTexture>& tex,
                const eWallType wall = eWallType::topLeft) :
        ePaintCall{px, py, tex},
        fType(type),
        fWallType(wall),
        fTX(tx), fTY(ty) {}

    eRenderCallType fType;
    eWallType fWallType;
    float fTX;
    float fTY;
};

struct eBlockerBase {
    eBlockerBase(const eBlockerBaseType type,
                 const float tx, const float ty) :
        fType(type),
        fTX(tx), fTY(ty) {}

    eBlockerBaseType fType;
    float fTX;
    float fTY;
};

struct eObjectLightBlocker : public eBlockerBase {
    eObjectLightBlocker(const float tx,
                        const float ty,
                        const float size) :
        eBlockerBase(eBlockerBaseType::object, tx, ty),
        fSize(size) {}
    float fSize;
};

struct eWallLightBlocker : public eBlockerBase {
    eWallLightBlocker(const float tx,
                      const float ty,
                      const eWallType dir,
                      const float wallMin,
                      const float wallMax) :
        eBlockerBase(eBlockerBaseType::wall, tx, ty),
        fDir(dir),
        fWallMin(wallMin),
        fWallMax(wallMax) {}

    eWallType fDir;
    float fWallMin;
    float fWallMax;
};

class eLightingHandler {
public:
    eLightingHandler(eTilesIterator& tileIterator);
    void initialize(SDL_Renderer* const r,
                    const int w, const int h,
                    const int tileW, const int tileH);
    void setLightness(const float l);
    void clear();

    void addLight(const eLight& light);
    void addBlocker(std::unique_ptr<eBlockerBase>& b);

    void setTopLeftTilePos(const ePointF& pos);

    void calculateLighting();
    void renderFloorLighting(SDL_Renderer * const r);

    void addRenderCall(std::unique_ptr<eRenderCall>& c);
    void renderAll(SDL_Renderer * const r);
private:
    eTilesIterator& mIterator;
    float mLightness = 0.f;
    std::vector<eLight> mLights;
    std::vector<std::unique_ptr<eBlockerBase>> mBlockers;
    std::vector<std::unique_ptr<eRenderCall>> mRenderCalls;

    int mTileDiv = 1;
    int mNDots = mTileDiv + 1;

    float mFeatherLen;
    int mBaseTileW;
    int mBaseTileH;
};

#endif // ELIGHTINGHANDLER_H

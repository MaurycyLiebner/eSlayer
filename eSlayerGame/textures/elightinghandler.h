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
    std::vector<std::unique_ptr<eRenderCall>> mRenderCalls;

    int mTileDiv = 1;
    int mNDots = mTileDiv + 1;

    float mFeatherLen;
    int mBaseTileW;
    int mBaseTileH;
};

#endif // ELIGHTINGHANDLER_H

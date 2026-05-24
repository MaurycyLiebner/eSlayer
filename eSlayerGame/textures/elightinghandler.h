#ifndef ELIGHTINGHANDLER_H
#define ELIGHTINGHANDLER_H

#include "epaintcall.h"

#include "etilesiterator.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/eterrstexturesdata.h>

#include <memory>

class eResolution;
class SDL_Renderer;

enum class eRenderCallType {
    unit, missile, item, object, wall, area
};

struct eRenderCall : public ePaintCall {
    eRenderCall(const eRenderCallType type,
                const float tx, const float ty,
                const float px, const float py,
                const std::shared_ptr<eTexture>& tex,
                const bool highlight,
                const bool shadow,
                const bool lighting = false,
                const SDL_FColor& colorMod = {1.f, 1.f, 1.f, 1.f},
                const eWallType wall = eWallType::topLeft,
                const bool transparent = false,
                const float scale = 1.f) :
        ePaintCall{px, py, tex},
        fType(type),
        fHighlight(highlight),
        fShadow(shadow),
        fColorMod(colorMod),
        fWallType(wall),
        fTransparent(transparent),
        fLighting(lighting),
        fTX(tx), fTY(ty),
        fScale(scale) {}

    eRenderCallType fType;
    bool fHighlight;
    bool fShadow;
    SDL_FColor fColorMod;
    eWallType fWallType;
    bool fTransparent;
    bool fLighting;
    float fTX;
    float fTY;
    float fScale;
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

    void render(SDL_Renderer* const r,
                const eRenderCall& c) const;
private:
    eTilesIterator& mIterator;
    float mLightness = 0.f;

    int mTileDiv = 1;
    int mNDots = mTileDiv + 1;

    float mFeatherLen;
    int mBaseTileW;
    int mBaseTileH;
};

#endif // ELIGHTINGHANDLER_H

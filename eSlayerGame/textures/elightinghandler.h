#ifndef ELIGHTINGHANDLER_H
#define ELIGHTINGHANDLER_H

#include "epaintcall.h"

#include "etilesiterator.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/eterrstexturesdata.h>

#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

class eResolution;
class SDL_Renderer;

enum class eRenderCallType {
    unit, missile, item, object, wall, area
};

struct eRenderCall : public ePaintCall {
    eRenderCall(const eRenderCallType type,
                const float tx, const float ty,
                const float px, const float py,
                const std::shared_ptr<eTexture>& tex) :
        ePaintCall{px, py, tex},
        fType(type),
        fTX(tx), fTY(ty) {}

    eRenderCallType fType;
    float fTX;
    float fTY;
    bool fHighlight = false;
    bool fShadow = false;
    SDL_FColor fColorMod{1.f, 1.f, 1.f, 1.f};
    eWallType fWallType = eWallType::topLeft;
    bool fTransparent = false;
    bool fLighting = false;
    float fScale = 1.f;
    float fObjSize = 1.f;
    bool fObjSplitLighting = false;
};

class eLightingHandler {
public:
    eLightingHandler(eTilesIterator& tileIterator);
    ~eLightingHandler();

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
    void workerLoop(const int shift);
    void calculate(const int shift, const int division);

    eTilesIterator& mIterator;
    float mLightness = 0.f;

    int mTileDiv = 1;
    int mNDots = mTileDiv + 1;

    float mFeatherLen;
    int mBaseTileW;
    int mBaseTileH;

    std::vector<SDL_Vertex> mVerts;
    std::vector<int> mIndices;

    // multithreading
    std::vector<std::thread> mThreads;

    std::mutex mMutex;
    std::condition_variable mWakeup;
    std::condition_variable mFinished;

    std::atomic<int> mRemaining{0};

    int mWorkerCount;
    int mDivision;
    int mGeneration = 0;
    bool mStop = false;
};

#endif // ELIGHTINGHANDLER_H

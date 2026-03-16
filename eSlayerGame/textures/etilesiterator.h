#ifndef ETILESITERATOR_H
#define ETILESITERATOR_H

#include "eSlayerHelpers/epoint.h"

#include <functional>

class eGameScreen;

using eVisibleTileFunc = std::function<void(const int x, const int y,
                                            const int px, const int py)>;

class eTilesIterator {
public:
    void initialize(eGameScreen* const game);

    void iterate(const eVisibleTileFunc& func) const;
private:
    ePoint mMin;
    float mPDX;
    float mPDY;
    int mTileW;
    int mTileH;
    int mTileMargin;
    int mDxMax;
    int mDyMax;
    int mMapWidth;
    int mMapHeight;
};

#endif // ETILESITERATOR_H

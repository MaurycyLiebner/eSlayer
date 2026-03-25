#ifndef ETILESITERATOR_H
#define ETILESITERATOR_H

#include "eSlayerHelpers/epoint.h"

#include <functional>

class eGameWidget;

using eVisibleTileFunc = std::function<void(const int x, const int y,
                                            const int px, const int py)>;

class eTilesIterator {
public:
    void initialize(eGameWidget* const game);

    void iterate(const eVisibleTileFunc& func) const;

private:
    eGameWidget* mGame = nullptr;
    ePoint mMin;
    int mTileMargin;
    int mDxMax;
    int mDyMax;
    int mMapWidth;
    int mMapHeight;
};

#endif // ETILESITERATOR_H

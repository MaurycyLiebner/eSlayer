#include "etilesiterator.h"

#include "../widgets/gameScreen/egamewidget.h"

void eTilesIterator::initialize(eGameWidget* const game) {
    const auto& pos = game->characterPos();
    const auto ipos = pos.floor();
    const ePointF p00{0.f, 0.f};
    const auto minF = game->pixelToTilePos(ipos, p00);
    mMin = minF.floor();
    const int width = game->width();
    const int height = game->height();
    mTileMargin = 5;
    const int tileW = game->tileWidth();
    const int tileH = game->tileHeight();
    mDxMax = width/tileW + 2;
    mDyMax = 2*height/tileH + 3;
    mMapWidth = game->mapWidth();
    mMapHeight = game->mapHeight();
}

void eTilesIterator::iterate(const eVisibleTileFunc& func) const {
    for(int dy = -1 - mTileMargin; dy < mDyMax + mTileMargin; dy++) {
        for(int dx = -1 - mTileMargin; dx < mDxMax + mTileMargin; dx++) {
            const int y = mMin.fY - dx + dy/2;
            if(y < 0 || y >= mMapHeight) continue;
            const int x = mMin.fX + dx + dy % 2 + dy/2;
            if(x < 0 || x >= mMapWidth) continue;
            func(x, y);
        }
    }
}

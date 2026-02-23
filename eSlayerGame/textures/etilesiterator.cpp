#include "etilesiterator.h"

#include "../screens/egamescreen.h"

void eTilesIterator::initialize(eGameScreen* const game) {
    const auto& pos = game->characterPos();
    mMin = game->pixelToTilePos(pos.floor(), {0., 0.}).floor();
    mPDX = pos.fX - int(pos.fX);
    mPDY = pos.fY - int(pos.fY);
    mTileW = game->tileWidth();
    mTileH = game->tileHeight();
    const int width = game->width();
    const int height = game->height();
    mDxMax = width/mTileW + 2;
    mDyMax = 2*height/mTileH + 3;
    mMapWidth = game->mapWidth();
    mMapHeight = game->mapHeight();
}

void eTilesIterator::iterate(const eVisibleTileFunc& func) const {
    for(int dy = -1; dy < mDyMax; dy++) {
        const int py = (dy + 1)*(mTileH + 1)/2 -
                       std::round((mPDX + mPDY)*mTileH/2.);
        for(int dx = -1; dx < mDxMax; dx++) {
            const int y = mMin.fY - dx + dy/2;
            if(y < 0 || y >= mMapHeight) continue;
            const int x = mMin.fX + dx + dy % 2 + dy/2;
            if(x < 0 || x >= mMapWidth) continue;
            const int px = (dy % 2) * mTileW/2 + dx*mTileW -
                           std::round((mPDX - mPDY)*mTileW/2.);
            func(x, y, px, py);
        }
    }
}

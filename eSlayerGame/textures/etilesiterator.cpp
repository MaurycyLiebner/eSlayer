#include "etilesiterator.h"

#include "../screens/egamescreen.h"

void eTilesIterator::initialize(eGameScreen* const game) {
    const auto& pos = game->characterPos();
    mMin = game->pixelToTilePos(pos.floor(), {0.f, 0.f}).floor();
    const int width = game->width();
    const int height = game->height();
    mTileMargin = 5;
    const int tileW = game->tileWidth();
    const int tileH = game->tileHeight();
    mDxMax = width/tileW + 2;
    mDyMax = 2*height/tileH + 3;
    mMapWidth = game->mapWidth();
    mMapHeight = game->mapHeight();
    mGame = game;
}

void eTilesIterator::iterate(const eVisibleTileFunc& func) const {
    for(int dy = -1 - mTileMargin; dy < mDyMax + mTileMargin; dy++) {
        for(int dx = -1 - mTileMargin; dx < mDxMax + mTileMargin; dx++) {
            const int y = mMin.fY - dx + dy/2;
            if(y < 0 || y >= mMapHeight) continue;
            const int x = mMin.fX + dx + dy % 2 + dy/2;
            if(x < 0 || x >= mMapWidth) continue;
            const auto pixel = mGame->tilePosToPixel(ePointF(x, y)).round();
            func(x, y, pixel.fX, pixel.fY);
        }
    }
}

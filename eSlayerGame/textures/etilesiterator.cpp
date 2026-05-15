#include "etilesiterator.h"

#include "../widgets/gameScreen/egamewidget.h"

void eTilesIterator::initialize(eGameWidget* const game) {
    mMapWidth = game->mapWidth();
    mMapHeight = game->mapHeight();

    mMap = std::vector<std::vector<eMapTile>>(
        mMapHeight, std::vector<eMapTile>(mMapWidth));
}

void eTilesIterator::nextIteration(eGameWidget* const game) {
    mIter++;
    const auto& pos = game->characterPos();
    const auto ipos = pos.floor();
    const ePointF p00{0.f, 0.f};
    const auto minF = game->pixelToTilePos(ipos, p00);
    const auto min = minF.floor();
    const int width = game->width();
    const int height = game->height();
    const int margin = 5;
    const int tileW = game->tileWidth();
    const int tileH = game->tileHeight();

    const int dxMin = -1 - margin;
    const int dxMax = width/tileW + 2 + margin;
    const int dyMin = -1 - margin;
    const int dyMax = 2*height/tileH + 3 + margin;

    const int w = dxMax - dxMin;
    const int h = dyMax - dyMin;
    mTiles.clear();
    mTiles.reserve(w*h);

    for(int dy = dyMin; dy < dyMax; dy++) {
        for(int dx = dxMin; dx < dxMax; dx++) {
            const int y = min.fY - dx + dy/2;
            if(y < 0 || y >= mMapHeight) continue;
            const int x = min.fX + dx + dy % 2 + dy/2;
            if(x < 0 || x >= mMapWidth) continue;
            const int id = mTiles.size();
            auto& mapTile = mMap[y][x];
            mapTile.fId = id;
            mapTile.fIter = mIter;
            const auto pos = ePointF(x, y);
            const auto pixel = game->tilePosToPixel(pos);
            const auto ipixel = pixel.round();
            const int px = ipixel.fX;
            const int py = ipixel.fY;
            mTiles.emplace_back(id, x, y, px, py);
        }
    }
}

void eTilesIterator::iterate(const eVisibleTileFunc& func) {
    for(auto& t : mTiles) {
        func(t);
    }
}

eTileInfo* eTilesIterator::getTile(
    const int x, const int y) {
    if(y < 0 || y >= mMapHeight) return nullptr;
    if(x < 0 || x >= mMapWidth) return nullptr;
    const auto& mapTile = mMap[y][x];
    if(mapTile.fIter != mIter) return nullptr;
    return &mTiles[mapTile.fId];
}

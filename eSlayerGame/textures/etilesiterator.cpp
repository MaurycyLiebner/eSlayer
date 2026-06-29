#include "etilesiterator.h"

#include "../widgets/gameScreen/egamewidget.h"

void eTilesIterator::initialize(eGameWidget* const game) {
    mMapWidth = game->mapWidth();
    mMapHeight = game->mapHeight();

    mTileMap = std::vector<std::vector<eMapTile>>(
        mMapHeight, std::vector<eMapTile>(mMapWidth));

    const int cellMapHeight = (mMapHeight + sCellSize - 1)/sCellSize;
    const int cellMapWidth = (mMapWidth + sCellSize - 1)/sCellSize;
    mCellMap = std::vector<std::vector<eMapTile>>(
        cellMapHeight, std::vector<eMapTile>(cellMapWidth));
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

    mCells.clear();
    mCells.reserve((w/sCellSize + 1)*(h/sCellSize + 1));

    mTiles.clear();
    mTiles.reserve(w*h);

    for(int dy = dyMin; dy < dyMax; dy++) {
        for(int dx = dxMin; dx < dxMax; dx++) {
            const int y = min.fY - dx + dy/2;
            if(y < 0 || y >= mMapHeight) continue;
            const int x = min.fX + dx + dy % 2 + dy/2;
            if(x < 0 || x >= mMapWidth) continue;
            const int id = mTiles.size();
            auto& mapTile = mTileMap[y][x];
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

void eTilesIterator::iterate(
    const int shift, const int division,
    const eVisibleTileFunc& func) {
    const int iMax = mTiles.size();
    for(int i = shift; i < iMax; i += division) {
        auto& t = mTiles[i];
        func(t);
    }
}

void eTilesIterator::iterateOverCells(const eCellFunc& func) {
    for(auto& c : mCells) {
        func(c);
    }
}

eTileInfo* eTilesIterator::getTile(
    const int x, const int y) {
    if(y < 0 || y >= mMapHeight) return nullptr;
    if(x < 0 || x >= mMapWidth) return nullptr;
    const auto& mapTile = mTileMap[y][x];
    if(mapTile.fIter != mIter) return nullptr;
    return &mTiles[mapTile.fId];
}

void eTilesIterator::addLight(
    const eLight& light) {
    int minCellX;
    int maxCellX;
    int minCellY;
    int maxCellY;
    lightCellRect(light, minCellX, maxCellX, minCellY, maxCellY);

    for(int cellX = minCellX; cellX <= maxCellX; cellX++) {
        for(int cellY = minCellY; cellY <= maxCellY; cellY++) {
            const auto c = requestCell(cellX, cellY);
            if(!c) continue;
            auto& lights = c->fLights;
            lights.emplace_back(light, minCellX, minCellY, maxCellX, maxCellY);
        }
    }
}

void eTilesIterator::addBlocker(
    const std::shared_ptr<eBlockerBase>& b) {
    const int xMin = b->fTX;
    const int yMin = b->fTY;
    const int cellMinX = xMin/sCellSize;
    const int cellMinY = yMin/sCellSize;
    const int xMax = b->fTX + b->fWidth - 0.01f;
    const int yMax = b->fTY + b->fHeight - 0.01f;
    const int cellMaxX = xMax/sCellSize;
    const int cellMaxY = yMax/sCellSize;
    for(int cellX = cellMinX; cellX <= cellMaxX; cellX++) {
        for(int cellY = cellMinY; cellY <= cellMaxY; cellY++) {
            const auto c = requestCell(cellX, cellY);
            if(!c) continue;
            c->fBlockers.emplace_back(b);
        }
    }
}

void eTilesIterator::lightCellRect(
    const eLight& light,
    int& minCellX, int& maxCellX,
    int& minCellY, int& maxCellY) const {
    const int minX = light.fTX - light.fRadius;
    const int maxX = light.fTX + light.fRadius;
    const int minY = light.fTY - light.fRadius;
    const int maxY = light.fTY + light.fRadius;

    minCellX = minX/sCellSize;
    maxCellX = maxX/sCellSize;
    minCellY = minY/sCellSize;
    maxCellY = maxY/sCellSize;
}

const eCell* eTilesIterator::getCellAtCellPos(
    const int cellX, const int cellY) const {
    if(cellX < 0 || cellX > mMapWidth/sCellSize) return nullptr;
    if(cellY < 0 || cellY > mMapHeight/sCellSize) return nullptr;
    const auto& mapCell = mCellMap[cellY][cellX];
    if(mapCell.fIter != mIter) return nullptr;
    const int id = mapCell.fId;
    return &mCells[id];
}

const eCell* eTilesIterator::getCellAtPos(
    const int x, const int y) const {
    const int cellX = x/sCellSize;
    const int cellY = y/sCellSize;
    return getCellAtCellPos(cellX, cellY);
}

eCell* eTilesIterator::requestCell(
    const int cellX, const int cellY) {
    if(cellX < 0 || cellX > mMapWidth/sCellSize) return nullptr;
    if(cellY < 0 || cellY > mMapHeight/sCellSize) return nullptr;
    auto& mapCell = mCellMap[cellY][cellX];
    if(mapCell.fIter == mIter) {
        const int id = mapCell.fId;
        return &mCells[id];
    } else {
        const int id = mCells.size();
        mapCell.fId = id;
        mapCell.fIter = mIter;
        auto& result = mCells.emplace_back();
        return &result;
    }
}

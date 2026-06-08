#ifndef ETILESITERATOR_H
#define ETILESITERATOR_H

#include "../elight.h"

#include <eSlayerHelpers/epoint.h>

#include <functional>
#include <memory>
#include <unordered_set>

class eGameWidget;

struct eMapTile {
    int fIter = -1;
    int fId = -1;
};

struct eTileInfo {
    eTileInfo(const int id,
              const int tx,
              const int ty,
              const int px,
              const int py) :
        fId(id),
        fTX(tx),
        fTY(ty),
        fPX(px),
        fPY(py) {}

    int fId;
    int fTX;
    int fTY;
    int fPX;
    int fPY;
    std::vector<float> fLighting;
};

struct eExtendedLight : public eLight {
    eExtendedLight(const eLight& light,
                   const int minCellX,
                   const int minCellY,
                   const int maxCellX,
                   const int maxCellY) :
        eLight{light},
        fMinCellX(minCellX),
        fMinCellY(minCellY),
        fMaxCellX(maxCellX),
        fMaxCellY(maxCellY) {}

    int fMinCellX;
    int fMinCellY;
    int fMaxCellX;
    int fMaxCellY;

    std::unordered_set<const eBlockerBase*> fBlockers;
};

struct eCell {
    std::vector<eExtendedLight> fLights;
    std::vector<std::unique_ptr<eBlockerBase>> fBlockers;
};

using eVisibleTileFunc = std::function<void(eTileInfo& tile)>;
using eCellFunc = std::function<void(eCell& tile)>;

class eTilesIterator {
public:
    void initialize(eGameWidget* const game);
    void nextIteration(eGameWidget* const game);
    void iterate(const eVisibleTileFunc& func);
    void iterate(const int shift, const int division,
                 const eVisibleTileFunc& func);
    void iterateOverCells(const eCellFunc& func);

    eTileInfo* getTile(const int x, const int y);
    eTileInfo& getTile(const int id) { return mTiles[id]; }

    int tileCount() const { return mTiles.size(); }

    void addLight(const eLight& light);
    void addBlocker(std::unique_ptr<eBlockerBase>& b);
    void lightCellRect(const eLight& light,
                       int& minCellX, int& maxCellX,
                       int& minCellY, int& maxCellY) const;
    const eCell* getCellAtPos(const int x, const int y) const;
    const eCell* getCellAtCellPos(const int cellX, const int cellY) const;
private:
    eCell* requestCell(const int cellX, const int cellY);

    int mIter = 0;
    std::vector<eTileInfo> mTiles;
    const int sCellSize = 4;
    std::vector<eCell> mCells;
    int mMapWidth = 0;
    int mMapHeight = 0;
    std::vector<std::vector<eMapTile>> mTileMap;
    std::vector<std::vector<eMapTile>> mCellMap;
};

#endif // ETILESITERATOR_H

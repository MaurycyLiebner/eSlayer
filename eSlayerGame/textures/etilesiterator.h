#ifndef ETILESITERATOR_H
#define ETILESITERATOR_H

#include <functional>

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
    float fPX;
    float fPY;
    std::vector<float> fLighting;
};

using eVisibleTileFunc = std::function<void(eTileInfo& tile)>;

class eTilesIterator {
public:
    void initialize(eGameWidget* const game);
    void nextIteration(eGameWidget* const game);
    void iterate(const eVisibleTileFunc& func);

    eTileInfo* getTile(const int x, const int y);
    eTileInfo& getTile(const int id) { return mTiles[id]; }

    int tileCount() const { return mTiles.size(); }
private:
    int mIter = 0;
    std::vector<eTileInfo> mTiles;
    int mMapWidth = 0;
    int mMapHeight = 0;
    std::vector<std::vector<eMapTile>> mMap;
};

#endif // ETILESITERATOR_H

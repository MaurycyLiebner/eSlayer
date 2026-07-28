#ifndef EPATHFINDERMAP_H
#define EPATHFINDERMAP_H

#include "eslayerhelpersexport.h"

#include "epoint.h"
#include "ecamprects.h"

#include <vector>
#include <functional>

struct ePathFinderTile {
    int fIter = 0;
    int fDist;

    bool fTop;
    bool fTopInitialized = false;
    bool fTopRight;
    bool fTopRightInitialized = false;
    bool fRight;
    bool fRightInitialized = false;
    bool fBottomRight;
    bool fBottomRightInitialized = false;
    bool fBottom;
    bool fBottomInitialized = false;
    bool fBottomLeft;
    bool fBottomLeftInitialized = false;
    bool fLeft;
    bool fLeftInitialized = false;
    bool fTopLeft;
    bool fTopLeftInitialized = false;
};

class ESLAYERHELPERS_API ePathFinderMap {
public:
    using eWalkable = std::function<bool(
        const ePointF& from, const ePointF& to)>;

    void initialize(const int w, const int h,
                    const eWalkable& walkable);

    bool walkable(const ePoint& from,
                  const int dx, const int dy);

    void erase(const ePoint& tile);

    void nextIter();
    int distance(const ePoint& tile) const;
    void setDistance(const ePoint& tile, const int dist);

    bool campAt(const ePointF& pos) const;
    bool campAtLine(const ePointF& from,
                    const ePointF& to) const;
    void addCampRect(const eRectF& rect);

    static const int sSubdivide;
    static ePoint posToTile(const ePointF& pos);
    static ePointF tileToPos(const ePoint& tile);
private:
    eWalkable mWalkable;

    eCampRects mCamp;

    int mWidth = 0;
    int mHeight = 0;

    int mIter = 0;
    std::vector<std::vector<ePathFinderTile>> mTiles;
};

#endif // EPATHFINDERMAP_H

#ifndef EOBSTACLESMAP_H
#define EOBSTACLESMAP_H

#include "eslayerhelpersexport.h"

#include "epoint.h"
#include "erect.h"

#include <vector>
#include <functional>

struct eObstacleTile {
    bool fInitialized = false;
    std::vector<eRectF> fWalk;
    std::vector<eRectF> fMissile;
};

class ESLAYERHELPERS_API eObstaclesMap {
public:
    using eObstacleFiller = std::function<
        bool(std::vector<eRectF>& walk,
             std::vector<eRectF>& missile,
             const int x, const int y)>;
    void initialize(const eObstacleFiller& filler,
                    const int w, const int h);
    void fillAll();

    eObstacleTile* tile(const int x, const int y);
    bool inside(const int x, const int y) const;
    void eraseTile(const int x, const int y);

    bool walkable(const ePointF& pos);
    bool walkable(const ePointF& from,
                  const ePointF& to);
    bool obstacle(const ePointF& pos);
    bool obstacle(const ePointF& from,
                  const ePointF& to);

    static const int sTileSize;
private:
    bool check(const ePointF& pos,
               const bool choice);
    bool check(const ePointF& from,
               const ePointF& to,
               const bool choice);

    int mWidth = 0;
    int mHeight = 0;
    eObstacleFiller mFiller;
    std::vector<std::vector<eObstacleTile>> mTiles;
};

#endif // EOBSTACLESMAP_H

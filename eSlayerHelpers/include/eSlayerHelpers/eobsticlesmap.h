#ifndef EOBSTICLESMAP_H
#define EOBSTICLESMAP_H

#include "eslayerhelpersexport.h"

#include "epoint.h"
#include "erect.h"

#include <vector>
#include <functional>

struct eObsticleTile {
    bool fInitialized = false;
    std::vector<eRectF> fWalk;
    std::vector<eRectF> fMissile;
};

class ESLAYERHELPERS_API eObsticlesMap {
public:
    using eObsticleFiller = std::function<
        void(std::vector<eRectF>& walk,
             std::vector<eRectF>& missile,
             const int x, const int y)>;
    void initialize(const eObsticleFiller& filler,
                    const int w, const int h);
    void fillAll();

    eObsticleTile* tile(const int x, const int y);
    bool inside(const int x, const int y) const;
    void eraseTile(const int x, const int y);

    bool walkable(const ePointF& pos);
    bool walkable(const ePointF& from,
                  const ePointF& to);
    bool obsticle(const ePointF& pos);
    bool obsticle(const ePointF& from,
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
    eObsticleFiller mFiller;
    std::vector<std::vector<eObsticleTile>> mTiles;
};

#endif // EOBSTICLESMAP_H

#ifndef EMOVEMENTHANDLER_H
#define EMOVEMENTHANDLER_H

#include "evec2.h"
#include "epathfinderpath.h"
#include "eslayerhelpersexport.h"

#include <functional>

using eWalkable = std::function<bool(const int x, const int y)>;
using eObsticle = std::function<bool(const int charId, const ePointF& p)>;

class ESLAYERHELPERS_API eMovementHandler {
public:

    const ePointF& pos() const { return mPos; }
    void setPos(const ePointF& pos) { mPos = pos; }
    double angle() const { return mAngle; }
    void setWalkable(const eWalkable& w);
    void setObsticle(const eObsticle& o);
    void setSpeed(const double s) { mSpeed = s; }

    bool moveTo(const ePointF& pos);
    bool increment();
    bool moveInDirection(const ePointF& pos);
private:
    bool tryMoveBy(eVec2d vec);
    void moveBy(const eVec2d& vec);

    int mCharId = 0;
    ePointF mPos{0., 0.};
    double mAngle = 0.;
    double mSpeed = 0.1;
    eWalkable mWalkable;
    eObsticle mObsticle;
    int mTileMoveSubdivision = 2;
    int mPathFindMargin = 40;
    ePathFinderPath mPath;
};

#endif // EMOVEMENTHANDLER_H

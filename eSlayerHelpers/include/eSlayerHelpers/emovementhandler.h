#ifndef EMOVEMENTHANDLER_H
#define EMOVEMENTHANDLER_H

#include "evec2.h"
#include "epathfinderpath.h"
#include "eslayerhelpersexport.h"

#include <functional>

using eWalkable = std::function<bool(const int x, const int y)>;

class ESLAYERHELPERS_API eMovementHandler {
public:

    const ePointF& pos() const { return mPos; }
    double angle() const { return mAngle; }
    void setWalkable(const eWalkable& w);
    void setSpeed(const double s) { mSpeed = s; }

    bool moveTo(const ePointF& pos);
    bool increment();
    bool moveInDirection(const ePointF& pos);
private:
    void moveBy(const eVec2d& vec);

    ePointF mPos{0., 0.};
    double mAngle = 0.;
    double mSpeed = 0.1;
    eWalkable mWalkable;
    int mTileMoveSubdivision = 2;
    int mPathFindMargin = 40;
    ePathFinderPath mPath;
};

#endif // EMOVEMENTHANDLER_H

#ifndef EMOVEMENTHANDLER_H
#define EMOVEMENTHANDLER_H

#include "evec2.h"
#include "eslayerhelpersexport.h"
#include "emovementgoal.h"

#include <functional>

class eUnitData;

using eWalkable = std::function<bool(const int x, const int y)>;

using eOtherHandler = std::function<void(const eUnitData&)>;
using eOtherIterator = std::function<void(const eOtherHandler&)>;

class ESLAYERHELPERS_API eMovementHandler {
public:
    void intialize(const eWalkable& w,
                   const eOtherIterator& iter,
                   const int charId);

    void setPathFindMargin(const int m)
    { mPathFindMargin = m; }

    int charId() const { return mCharId; }

    const ePointF& pos() const { return mPos; }
    void setPos(const ePointF& pos) { mPos = pos; }
    double angle() const { return mAngle; }
    void setSpeed(const double s) { mSpeed = s; }
    void setRadius(const double r) { mRadius = r; }

    bool increment(const double by);

    void stopMoving();
    bool moveTo(const ePointF& pos);
    void moveInDirection(const ePointF& pos);
private:
    void moveBy(const eVec2d& vec);
    bool walkable(const ePointF& pos) const;

    int mCharId = 0;
    eWalkable mWalkable;
    eOtherIterator mOtherIterator;

    eMovementGoal mGoal;

    ePointF mPos{0., 0.};
    eVec2d mVel{0., 0.};

    double mRadius = 0.4;
    double mSpeed = 0.1;
    double mStuckTimer = 0.;

    double mAngle = 0.;
    int mTileMoveSubdivision = 2;
    int mPathFindMargin = 40;
    double mWaypointReachDist = 0.2;
    double mNearbyUnits = 1.5;
};

#endif // EMOVEMENTHANDLER_H

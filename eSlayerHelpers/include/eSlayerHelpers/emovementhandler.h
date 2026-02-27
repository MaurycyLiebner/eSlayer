#ifndef EMOVEMENTHANDLER_H
#define EMOVEMENTHANDLER_H

#include "evec2.h"
#include "eslayerhelpersexport.h"
#include "emovementgoal.h"

#include <functional>
#include <queue>

using eWalkable = std::function<bool(const int x, const int y)>;
using eObsticle = std::function<bool(const int charId, const ePointF& p)>;


class ESLAYERHELPERS_API eMovementHandler {
public:
    void intialize(const int charId);

    void setPathFindMargin(const int m)
    { mPathFindMargin = m; }

    const ePointF& pos() const { return mPos; }
    void setPos(const ePointF& pos) { mPos = pos; }
    double angle() const { return mAngle; }
    void setWalkable(const eWalkable& w);
    void setObsticle(const eObsticle& o);
    void setSpeed(const double s) { mSpeed = s; }
    void setDelay(const int d) { mDelay = d; }

    bool increment(const double by);

    void stopMoving();
    bool moveTo(const ePointF& pos);
    void moveInDirection(const ePointF& pos);
private:
    void planMovement(const ePointF& to);
    bool incMovement(const double by);
    void clearPlanned();

    bool tryMoveBy(eVec2d vec);
    void moveBy(const eVec2d& vec);

    int mDelay = 0;
    std::queue<ePointF> mPlanned;

    eMovementGoal mGoal;

    int mCharId = 0;
    ePointF mPos{0., 0.};
    double mAngle = 0.;
    double mSpeed = 0.1;
    eWalkable mWalkable;
    eObsticle mObsticle;
    int mTileMoveSubdivision = 2;
    int mPathFindMargin = 40;
};

#endif // EMOVEMENTHANDLER_H

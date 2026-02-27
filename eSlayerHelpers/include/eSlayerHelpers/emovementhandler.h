#ifndef EMOVEMENTHANDLER_H
#define EMOVEMENTHANDLER_H

#include "evec2.h"
#include "eslayerhelpersexport.h"
#include "emovementgoal.h"
#include "eidpointf.h"

#include <functional>
#include <queue>

using eWalkable = std::function<bool(const int x, const int y)>;
using eObsticle = std::function<bool(const int charId, const ePointF& p)>;


class ESLAYERHELPERS_API eMovementHandler {
public:
    void intialize(const eWalkable& w,
                   const eObsticle& o,
                   const int charId);

    void setPathFindMargin(const int m)
    { mPathFindMargin = m; }

    const ePointF& pos() const { return mPos; }
    void setPos(const ePointF& pos) { mPos = pos; }
    double angle() const { return mAngle; }
    void setSpeed(const double s) { mSpeed = s; }
    void setDelay(const int d) { mDelay = d; }

    bool increment(const double by);

    void setDivergeAngle(const double a);

    void stopMoving();
    bool moveTo(const ePointF& pos);
    void moveInDirection(const ePointF& pos);

    void pushPlanned(std::queue<eIdPointF> planned);
    const std::queue<eIdPointF>& planned() const { return mPlanned; }
private:
    void planMovement(const ePointF& to);
    bool incMovement(const double by);
    void clearPlanned();

    bool tryMove(eVec2d& vec);
    bool tryMoveBy(eVec2d vec);
    void moveBy(const eVec2d& vec);

    int mCharId = 0;
    eWalkable mWalkable;
    eObsticle mObsticle;

    int mDelay = 0;
    int mNextPlannedId = 0;
    std::queue<eIdPointF> mPlanned;

    eMovementGoal mGoal;

    ePointF mPos{0., 0.};
    double mAngle = 0.;
    double mSpeed = 0.1;
    int mTileMoveSubdivision = 2;
    int mPathFindMargin = 40;
    double mMaxDivergeAngle = 15.;
};

#endif // EMOVEMENTHANDLER_H

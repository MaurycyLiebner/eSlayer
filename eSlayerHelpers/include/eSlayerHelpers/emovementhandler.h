#ifndef EMOVEMENTHANDLER_H
#define EMOVEMENTHANDLER_H

#include "eslayerhelpersexport.h"

#include "evec2.h"
#include "emovementgoal.h"

#include <eSlayerHelpers/eteamid.h>

#include <functional>

class eUnitData;

using eOtherHandler = std::function<void(const eUnitData&)>;
using eOtherIterator = std::function<void(
    const ePointF& pos, const float dist,
    const eOtherHandler& iter)>;

class ESLAYERHELPERS_API eMovementHandler {
public:
    eMovementHandler(ePointF& pos, float& angle);

    void intialize(const eWalkable& w,
                   const eOtherIterator& iter,
                   const int charId,
                   const eTeamId teamId);

    void setPathFindMargin(const int m)
    { mPathFindMargin = m; }

    int charId() const { return mCharId; }

    const ePointF& pos() const { return mPos; }
    void setPos(const ePointF& pos) { mPos = pos; }
    float angle() const { return mAngle; }
    float speed() const { return mSpeed; }
    void setSpeed(const float s) { mSpeed = s; }
    void setRadius(const float r);
    void setMoveRandom(const float r) { mMoveRandom = r; }
    float stuckTime() const { return mStuckTimer; }

    bool increment(const float by);

    bool moving() const { return mGoal.moving(); }
    eMovementGoalType goalType() const { return mGoal.type(); }
    void stopMoving();
    bool moveTo(const ePointF& pos);
    void moveInDirection(const ePointF& pos);
    bool moveInDirectionIfClearPath(const ePointF& pos);

    static int sChooseAnim(const int normal,
                           const int aggressive,
                           const bool isAggressive);

private:
    bool walkable(const ePointF& pos) const;
    bool walkable(const ePointF& from, const ePointF& to) const;
    ePathFinderPath smoothPath(const ePathFinderPath& path);

    ePointF& mPos;
    float& mAngle;

    int mCharId = 0;
    eTeamId mTeamId = eTeamId::neutralHostile;
    eWalkable mWalkable;
    eOtherIterator mOtherIterator;

    eMovementGoal mGoal;

    eVec2f mVel{0.f, 0.f};

    float mRadius = 0.4f;
    float mSpeed = 0.1f;
    float mStuckTimer = 0.f;

    int mTileMoveSubdivision = 2;
    int mPathFindMargin = 40;
    float mWaypointReachDist = 0.2f;
    float mNearbyUnits = 1.5f;

    float mMoveRandom = 0.05f;

    eVec2f mRandomOffset{0.f, 0.f};
    float mRandomTimer = 0.f;
};

#endif // EMOVEMENTHANDLER_H

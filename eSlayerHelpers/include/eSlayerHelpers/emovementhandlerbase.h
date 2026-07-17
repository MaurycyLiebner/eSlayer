#ifndef EMOVEMENTHANDLERBASE_H
#define EMOVEMENTHANDLERBASE_H

#include "eslayerhelpersexport.h"

#include "evec2.h"
#include "emovementgoal.h"

#include <eSlayerHelpers/eteamid.h>
#include <eSlayerHelpers/epathfindermap.h>

#include <functional>

class eUnitData;

using eOtherHandler = std::function<void(const eUnitData&)>;
using eOtherIterator = std::function<void(
    const ePointF& pos, const float dist,
    const eOtherHandler& iter)>;

class ESLAYERHELPERS_API eMovementHandlerBase {
public:
    eMovementHandlerBase(const eUnitData& u);

    void intialize(const eWalkablePos& wPos,
                   const eWalkablePath& wPath,
                   const eOtherIterator& iter,
                   const uint32_t charId,
                   const eTeamId teamId,
                   ePathFinderMap& map);

    uint32_t charId() const { return mCharId; }

    const ePointF& pos() const { return mPos; }
    float angle() const { return mAngle; }
    float speed() const { return mSpeed; }
    void setSpeed(const float s) { mSpeed = s; }
    void setMoveRandom(const float r) { mMoveRandom = r; }
    float stuckTime() const { return mStuckTimer; }
    float pushTime() const { return mPushTimer; }

    bool moving() const { return mGoal.moving(); }
    eMovementGoalType goalType() const { return mGoal.type(); }
    void stopMoving();
    bool moveTo(const std::vector<ePointF>& pos,
                const bool foundOnly = false);
    void moveInDirection(const ePointF& pos);
    bool moveInDirectionIfClearPath(const ePointF& pos);

    static int sChooseAnim(const int normal,
                           const int aggressive,
                           const bool isAggressive);
protected:
    bool increment(const float by,
                   float& angle,
                   ePointF& pos);
private:
    bool walkable(const ePointF& pos) const;

    const ePointF& mPos;
    const float& mAngle;
    const float& mRadius;
    ePathFinderMap* mMap = nullptr;

    uint32_t mCharId = 0;
    eTeamId mTeamId = eTeamId::neutralHostile;
    eWalkablePos mWalkablePos;
    eWalkablePath mWalkablePath;
    eOtherIterator mOtherIterator;

    eMovementGoal mGoal;

    eVec2f mVel{0.f, 0.f};

    float mSpeed = 0.1f;
    float mStuckTimer = 0.f;

    float mPushTimer = 0.f;

    float mWaypointReachDist = 0.2f;
    float mNearbyUnits = 1.5f;

    float mMoveRandom = 0.05f;

    eVec2f mRandomOffset{0.f, 0.f};
    float mRandomTimer = 0.f;
};

#endif // EMOVEMENTHANDLERBASE_H

#include "eSlayerHelpers/emovementhandlerbase.h"

#include "eSlayerHelpers/epathfinder.h"
#include "eSlayerHelpers/eunitdata.h"
#include "eSlayerHelpers/evec2.h"

#include <cstdio>

eMovementHandlerBase::eMovementHandlerBase(
    const eUnitData& u,
    ePathFinderMap& map) :
    mPos(u.fPos),
    mAngle(u.fAngle),
    mRadius(u.fRadius),
    mMap(map) {}

void eMovementHandlerBase::intialize(
    const eWalkablePos& wPos,
    const eWalkablePath& wPath,
    const eOtherIterator& iter,
    const int charId,
    const eTeamId teamId) {
    mCharId = charId;
    mTeamId = teamId;
    mWalkablePos = wPos;
    mWalkablePath = wPath;
    mOtherIterator = iter;
}

bool eMovementHandlerBase::moveTo(
    const std::vector<ePointF>& pos,
    const bool foundOnly) {
    bool found;
    const int maxDist = 20*ePathFinderMap::sSubdivide;
    auto path = ePathFinder::findPath(mMap, mPos, pos, maxDist, found);
    if(foundOnly && !found) return false;
    if(path.empty()) return false;
    {
        ePathFinderPath smooth;
        smooth.push_back(path.front());

        size_t i = 0;

        while(i < path.size() - 1) {
            size_t best = i + 1;

            for(size_t j = path.size() - 1; j > i + 1; --j) {
                if(mWalkablePath(path[i], path[j])) {
                    best = j;
                    break;
                }
            }

            smooth.push_back(path[best]);
            i = best;
        }

        path = std::move(smooth);
    }

    mGoal.moveOnPath(path);
    return found;
}

void eMovementHandlerBase::moveInDirection(
    const ePointF& pos) {
    mGoal.moveInDir(pos);
}

bool eMovementHandlerBase::moveInDirectionIfClearPath(
    const ePointF& pos) {
    const bool r = mWalkablePath(mPos, pos);
    if(r) moveInDirection(pos);
    return r;
}

int eMovementHandlerBase::sChooseAnim(
    const int normal,
    const int aggressive,
    const bool isAggressive) {
    if(isAggressive) {
        return (aggressive != -1) ? aggressive : normal;
    } else {
        return (normal != -1) ? normal : aggressive;
    }
}

bool eMovementHandlerBase::walkable(
    const ePointF& pos) const {
    bool walkable = true;
    mOtherIterator(pos, 1.f, [&](const eUnitData& other) {
        if(!walkable) return;
        if(other.fCharId == mCharId) return;
        if(other.fTeamId == mTeamId) return;
        if(other.fHealth <= 0) return;
        const eVec2f diff = ePointF::vector(pos, other.fPos);
        const float dist = diff.length();
        if(dist > mNearbyUnits) return;
        const float minDist = 0.4f*(mRadius + other.fRadius);
        walkable = walkable && (dist > minDist || dist < 0.0001f);
    });
    if(!walkable) return false;
    return mWalkablePos(pos);
}

bool eMovementHandlerBase::increment(
    const float by,
    float& angle,
    ePointF& pos) {
    angle = mAngle;
    pos = mPos;
    ePointF to;
    const bool r = mGoal.goal(to);
    if(!r) return false;
    switch(mGoal.type()) {
    case eMovementGoalType::dir: {
        if(ePointF::distance(pos, to) < 0.05f) {
            mGoal.stopMoving();
            return false;
        }
    } break;
    case eMovementGoalType::path: {
        if(ePointF::distance(pos, to) < mWaypointReachDist) {
            mGoal.nextWaypoint();
        }
    } break;
    default:
        return false;
    }
    auto desiredDir = ePointF::vector(to, pos);
    desiredDir.normalize();

    eVec2f separation{0.f, 0.f};
    eVec2f avoid{0.f, 0.f};

    mOtherIterator(pos, 2.f, [&](const eUnitData& other) {
        if(other.fCharId == mCharId) return;
        if(other.fTeamId != mTeamId) return;
        if(other.fHealth <= 0) return;
        eVec2f diff = ePointF::vector(pos, other.fPos);
        const float dist = diff.length();
        if(dist > mNearbyUnits) return;
        diff.normalize();

        const float minDist = mRadius + other.fRadius;
        if(dist < minDist && dist > 0.0001f) {
            separation += diff*(minDist - dist);
        }
    });

    auto moveDir = desiredDir*1.0f +
                   separation*1.5f;

    const bool addRandom = separation.length() > 0.1f;
    if(addRandom) {
        mRandomTimer += by;
        if(mRandomTimer > 12.f || mRandomOffset.length() == 0.f) {
            mRandomTimer = 0.f;
            mRandomOffset = eVec2f::random() * mMoveRandom;
        }
        moveDir = moveDir + mRandomOffset;
    } else {
        mRandomTimer = 0.f;
        mRandomOffset = eVec2f{0.f, 0.f};
    }
    if(moveDir.length() > 0) {
        moveDir.normalize();
    }

    const float distToGoal = ePointF::distance(pos, to);
    const float slowRadius = 0.5f;

    float speed = mSpeed;
    if(distToGoal < slowRadius) {
        speed *= distToGoal / slowRadius;
    }

    const auto targetVel = moveDir*speed;
    const float blendFactor = 0.2f;
    mVel = mVel*(1.0f - blendFactor) + targetVel*blendFactor;

    const float progress = eVec2f::dot(mVel, desiredDir);
    if(progress < 0.1f*speed) {
        mStuckTimer += by;
    } else {
        mStuckTimer = 0.f;
    }

    if(mVel.length() > 0.001f) {
        const float targetAngle = mVel.angle();
        float angleDiff = targetAngle - angle;
        while(angleDiff > 180.f) angleDiff -= 360.f;
        while(angleDiff < -180.f) angleDiff += 360.f;
        const float angleBlend = 0.3f;
        angle += angleDiff * angleBlend;
        while(angle < 0.f) angle += 360.f;
        while(angle >= 360.f) angle -= 360.f;
    }

    const auto newPos = pos + mVel*by;
    if(walkable(newPos)) {
        pos = newPos;
        mPushTimer = 0.f;
    } else {
        mPushTimer += by;
        const auto tryX = ePointF{newPos.fX, pos.fY};
        const auto tryY = ePointF{pos.fX, newPos.fY};
        if(walkable(tryX)) {
            pos = tryX;
        } else if(walkable(tryY)) {
            pos = tryY;
        } else {
            stopMoving();
        }
    }

    return true;
}

void eMovementHandlerBase::stopMoving() {
    mStuckTimer = 0.f;
    mGoal.stopMoving();
}

#include "eSlayerHelpers/emovementhandler.h"

#include "eSlayerHelpers/epathfinder.h"
#include "eSlayerHelpers/eunitdata.h"
#include "eSlayerHelpers/evec2.h"

#include <cstdio>

eMovementHandler::eMovementHandler(
    eUnitData& u, ePathFinderMap& map) :
    mUnit(u),
    mPos(u.fPos),
    mAngle(u.fAngle),
    mMap(map) {}

void eMovementHandler::intialize(const eWalkablePos& wPos,
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

void eMovementHandler::setRadius(const float r) {
    mRadius = r;
}

bool eMovementHandler::moveTo(const ePointF& dst) {
    bool found;
    const int maxDist = 20*ePathFinderMap::sSubdivide;
    auto path = ePathFinder::findPath(mMap, mPos, dst, maxDist, found);
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

void eMovementHandler::moveInDirection(const ePointF& pos) {
    mGoal.moveInDir(pos);
}

bool eMovementHandler::moveInDirectionIfClearPath(const ePointF& pos) {
    const bool r = mWalkablePath(mPos, pos);
    if(r) moveInDirection(pos);
    return r;
}

int eMovementHandler::sChooseAnim(const int normal,
                                  const int aggressive,
                                  const bool isAggressive) {
    if(isAggressive) {
        return (aggressive != -1) ? aggressive : normal;
    } else {
        return (normal != -1) ? normal : aggressive;
    }
}

bool eMovementHandler::walkable(const ePointF& pos) const {
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

bool eMovementHandler::increment(const float by) {
    ePointF to;
    const bool r = mGoal.goal(to);
    if(!r) return false;
    switch(mGoal.type()) {
    case eMovementGoalType::dir: {
        if(ePointF::distance(mPos, to) < 0.05f) {
            mGoal.stopMoving();
            return false;
        }
    } break;
    case eMovementGoalType::path: {
        if(ePointF::distance(mPos, to) < mWaypointReachDist) {
            mGoal.nextWaypoint();
        }
    } break;
    default:
        return false;
    }
    auto desiredDir = ePointF::vector(to, mPos);
    desiredDir.normalize();

    eVec2f separation{0.f, 0.f};
    eVec2f avoid{0.f, 0.f};

    mOtherIterator(mPos, 2.f, [&](const eUnitData& other) {
        if(other.fCharId == mCharId) return;
        if(other.fTeamId != mTeamId) return;
        if(other.fHealth <= 0) return;
        eVec2f diff = ePointF::vector(mPos, other.fPos);
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

    const float distToGoal = ePointF::distance(mPos, to);
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
        float angleDiff = targetAngle - mAngle;
        while(angleDiff > 180.f) angleDiff -= 360.f;
        while(angleDiff < -180.f) angleDiff += 360.f;
        const float angleBlend = 0.3f;
        mAngle += angleDiff * angleBlend;
        while(mAngle < 0.f) mAngle += 360.f;
        while(mAngle >= 360.f) mAngle -= 360.f;
    }

    const auto newPos = mPos + mVel*by;
    if(walkable(newPos)) {
        mPos = newPos;
    } else {
        const auto tryX = ePointF{newPos.fX, mPos.fY};
        const auto tryY = ePointF{mPos.fX, newPos.fY};
        if(walkable(tryX)) {
            mPos.fX = tryX.fX;
        } else if(walkable(tryY)) {
            mPos.fY = tryY.fY;
        } else {
            stopMoving();
        }
    }

    return true;
}

void eMovementHandler::stopMoving() {
    mStuckTimer = 0.f;
    mGoal.stopMoving();
}

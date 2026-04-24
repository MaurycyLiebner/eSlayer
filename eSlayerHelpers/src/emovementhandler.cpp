#include "eSlayerHelpers/emovementhandler.h"

#include "eSlayerHelpers/epathfinder.h"
#include "eSlayerHelpers/eunitdata.h"
#include "eSlayerHelpers/evec2.h"

#include <cstdio>

eMovementHandler::eMovementHandler(ePointF& pos, float& angle) :
    mPos(pos), mAngle(angle) {}

void eMovementHandler::intialize(const eWalkable& w,
                                 const eOtherIterator& iter,
                                 const int charId,
                                 const eTeamId teamId) {
    mCharId = charId;
    mTeamId = teamId;
    mWalkable = w;
    mOtherIterator = iter;
}

void eMovementHandler::setRadius(const float r) {
    mRadius = r;
    if(r > 0.5f) mTileMoveSubdivision = 1;
    else mTileMoveSubdivision = 2;
}

bool eMovementHandler::moveTo(const ePointF& dst) {
    const int margin = mPathFindMargin;
    const float subdivision = mTileMoveSubdivision;
    const int dim = 2*margin + 1;
    ePathFinderMap map(0, 0, dim, dim);
    const auto iPos = (mPos * subdivision).round();
    for(int sx = 0; sx < dim; sx++) {
        for(int sy = 0; sy < dim; sy++) {
            const int x = (iPos.fX + sx - margin)/mTileMoveSubdivision;
            const int y = (iPos.fY + sy - margin)/mTileMoveSubdivision;
            map.set({sx, sy}, mWalkable(x, y));
        }
    }
    bool found;
    const ePoint from{margin, margin};
    const auto ipos = (dst * subdivision).round();
    const ePoint to{margin + (ipos.fX - iPos.fX),
                    margin + (ipos.fY - iPos.fY)};
    auto path = ePathFinder::findPath(map, from, to, found);
    for(auto& step : path) {
        step.fX -= margin;
        step.fY -= margin;
        step.fX /= subdivision;
        step.fY /= subdivision;
        step.fX += (std::round(mPos.fX*subdivision) + 0.5f)/subdivision;
        step.fY += (std::round(mPos.fY*subdivision) + 0.5f)/subdivision;
    }
    {
            path.emplace(path.begin(), mPos);
            for(int i = 0; i < path.size() - 2; i++) {
                const auto& from = path[i];
                int j = path.size() - 1;
                for(; j > i + 1; j--) {
                    const bool r = walkable(from, path[j]);
                    if(r) break;
                }
                path.erase(path.begin() + i + 1, path.begin() + j);
            }
            path.erase(path.begin());
    }
    mGoal.moveOnPath(path);
    return found;
}

void eMovementHandler::moveInDirection(const ePointF& pos) {
    mGoal.moveInDir(pos);
}

bool eMovementHandler::walkable(const ePointF& from, const ePointF& to) const {
    const float x0 = from.fX;
    const float y0 = from.fY;
    const float x1 = to.fX;
    const float y1 = to.fY;

    int x = (int)std::floor(x0);
    int y = (int)std::floor(y0);

    const int endX = (int)std::floor(x1);
    const int endY = (int)std::floor(y1);

    const float dx = x1 - x0;
    const float dy = y1 - y0;

    const int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    const int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

    const float tDeltaX = (stepX != 0) ? std::abs(1.0f / dx) :
                               std::numeric_limits<float>::infinity();
    const float tDeltaY = (stepY != 0) ? std::abs(1.0f / dy) :
                               std::numeric_limits<float>::infinity();

    const float nextBoundaryX = (stepX > 0)
                                     ? (std::floor(x0) + 1.0f)
                                     : std::floor(x0);

    const float nextBoundaryY = (stepY > 0)
                                     ? (std::floor(y0) + 1.0f)
                                     : std::floor(y0);

    float tMaxX = (stepX != 0) ? (nextBoundaryX - x0) / dx :
                       std::numeric_limits<float>::infinity();

    float tMaxY = (stepY != 0) ? (nextBoundaryY - y0) / dy :
                       std::numeric_limits<float>::infinity();

    while(true) {
        const bool r = mWalkable(x, y);
        if(!r) return false;

        if(x == endX && y == endY) {
            break;
        }

        if(tMaxX < tMaxY) {
            tMaxX += tDeltaX;
            x += stepX;
        } else {
            tMaxY += tDeltaY;
            y += stepY;
        }
    }
    return true;
}

bool eMovementHandler::moveInDirectionIfClearPath(const ePointF& pos) {
    const bool r = walkable(mPos, pos);
    if(r) {
        moveInDirection(pos);
    }
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
    const auto ipos = pos.floor();
    return mWalkable(ipos.fX, ipos.fY);
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

    float speedFactor = 1.0f;
    if(distToGoal < slowRadius) {
        speedFactor = distToGoal / slowRadius;
    }

    const auto targetVel = moveDir*mSpeed*speedFactor;
    const float blendFactor = 0.2f;
    mVel = mVel*(1.0f - blendFactor) + targetVel*blendFactor;

    const float progress = eVec2f::dot(mVel, desiredDir);
    if(progress < 0.1f*mSpeed) {
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

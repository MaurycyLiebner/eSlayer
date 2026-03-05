#include "eSlayerHelpers/emovementhandler.h"

#include "eSlayerHelpers/epathfinder.h"
#include "eSlayerHelpers/eunitdata.h"
#include "eSlayerHelpers/evec2.h"

#include <cstdio>

void eMovementHandler::intialize(const eWalkable& w,
                                 const eOtherIterator& iter,
                                 const int charId) {
    mCharId = charId;
    mWalkable = w;
    mOtherIterator = iter;
}

bool eMovementHandler::moveTo(const ePointF& dst) {
    const int margin = mPathFindMargin;
    const double subdivision = mTileMoveSubdivision;
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
        step.fSrc.fX -= margin;
        step.fSrc.fY -= margin;
        step.fDst.fX -= margin;
        step.fDst.fY -= margin;
        step.fSrc.fX /= subdivision;
        step.fSrc.fY /= subdivision;
        step.fDst.fX /= subdivision;
        step.fDst.fY /= subdivision;
        step.fSrc.fX += std::round(mPos.fX*subdivision)/subdivision;
        step.fSrc.fY += std::round(mPos.fY*subdivision)/subdivision;
        step.fDst.fX += std::round(mPos.fX*subdivision)/subdivision;
        step.fDst.fY += std::round(mPos.fY*subdivision)/subdivision;
    }
    mGoal.moveOnPath(path);
    return found;
}

void eMovementHandler::moveInDirection(const ePointF& pos) {
    mGoal.moveInDir(pos);
}

void eMovementHandler::moveBy(const eVec2d& vec) {
    mPos.fX += vec.x;
    mPos.fY += vec.y;
    mAngle = vec.angle();
}

bool eMovementHandler::walkable(const ePointF& pos) const {
    const auto ipos = pos.floor();
    return mWalkable(ipos.fX, ipos.fY);
}

bool eMovementHandler::increment(const double by) {
    ePointF to;
    const bool r = mGoal.goal(to);
    if(!r) return false;
    switch(mGoal.type()) {
    case eMovementGoalType::dir: {
        if(ePointF::distance(mPos, to) < 0.01) {
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

    eVec2d separation{0., 0.};
    eVec2d avoid{0., 0.};

    mOtherIterator([&](const eUnitData& other) {
        if(other.fCharId == mCharId) return;
        eVec2d diff = ePointF::vector(mPos, other.fPos);
        const double dist = diff.length();
        if(dist > mNearbyUnits) return;
        diff.normalize();
        {
            const double minDist = mRadius + other.fRadius;
            if(dist < minDist && dist > 0.0001) {
                separation += diff*(minDist - dist);
            }
        }
        {
            const auto relPos = ePointF::vector(other.fPos, mPos);
            auto normRelPos = relPos;
            normRelPos.normalize();
            const auto relVel = mVel - other.fVel;
            if(eVec2d::dot(relPos, relVel) < 0) {
                avoid -= normRelPos;
            }
        }
    });

    auto moveDir = desiredDir*1.0 +
                   separation*1.5 +
                   avoid*0.35 +
                   eVec2d::random()*0.05;
    if(moveDir.length() > 0) {
        moveDir.normalize();
    }
    mVel = moveDir*mSpeed;
    mAngle = mVel.angle();

    const auto newPos = mPos + mVel*by;
    if(walkable(newPos)) {
        mPos = newPos;
    } else {
        const auto tryX = ePointF{newPos.fX, mPos.fY};
        const auto tryY = ePointF{mPos.fX, newPos.fY};
        if(walkable(tryX)) mPos.fX = tryX.fX;
        if(walkable(tryY)) mPos.fY = tryY.fY;
    }

    return true;
}

void eMovementHandler::stopMoving() {
    mGoal.stopMoving();
}

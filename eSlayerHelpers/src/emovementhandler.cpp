#include "eSlayerHelpers/emovementhandler.h"

#include "eSlayerHelpers/epathfinder.h"
#include "eSlayerHelpers/eunitdata.h"
#include "eSlayerHelpers/evec2.h"

#include <cstdio>

void eMovementHandler::intialize(const eWalkable& w,
                                 const eOtherIterator& iter,
                                 const int charId,
                                 const int teamId) {
    mCharId = charId;
    mTeamId = teamId;
    mWalkable = w;
    mOtherIterator = iter;
}

void eMovementHandler::setRadius(const double r) {
    mRadius = r;
    if(r > 0.5) mTileMoveSubdivision = 1;
    else mTileMoveSubdivision = 2;
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
        step.fSrc.fX += (std::round(mPos.fX*subdivision) + 0.5)/subdivision;
        step.fSrc.fY += (std::round(mPos.fY*subdivision) + 0.5)/subdivision;
        step.fDst.fX += (std::round(mPos.fX*subdivision) + 0.5)/subdivision;
        step.fDst.fY += (std::round(mPos.fY*subdivision) + 0.5)/subdivision;
    }
    mGoal.moveOnPath(path);
    return found;
}

void eMovementHandler::moveInDirection(const ePointF& pos) {
    mGoal.moveInDir(pos);
}

bool eMovementHandler::walkable(const ePointF& from, const ePointF& to) const {
    const double x0 = from.fX;
    const double y0 = from.fY;
    const double x1 = to.fX;
    const double y1 = to.fY;

    int x = (int)std::floor(x0);
    int y = (int)std::floor(y0);

    const int endX = (int)std::floor(x1);
    const int endY = (int)std::floor(y1);

    const double dx = x1 - x0;
    const double dy = y1 - y0;

    const int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    const int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

    const double tDeltaX = (stepX != 0) ? std::abs(1.0 / dx) :
                               std::numeric_limits<double>::infinity();
    const double tDeltaY = (stepY != 0) ? std::abs(1.0 / dy) :
                               std::numeric_limits<double>::infinity();

    const double nextBoundaryX = (stepX > 0)
                                     ? (std::floor(x0) + 1.0)
                                     : std::floor(x0);

    const double nextBoundaryY = (stepY > 0)
                                     ? (std::floor(y0) + 1.0)
                                     : std::floor(y0);

    double tMaxX = (stepX != 0) ? (nextBoundaryX - x0) / dx :
                       std::numeric_limits<double>::infinity();

    double tMaxY = (stepY != 0) ? (nextBoundaryY - y0) / dy :
                       std::numeric_limits<double>::infinity();

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

bool eMovementHandler::walkable(const ePointF& pos) const {
    bool walkable = true;
    mOtherIterator([&](const eUnitData& other) {
        if(!walkable) return;
        if(other.fCharId == mCharId) return;
        if(other.fTeamId == mTeamId) return;
        const eVec2d diff = ePointF::vector(pos, other.fPos);
        const double dist = diff.length();
        if(dist > mNearbyUnits) return;
        const double minDist = 0.4*(mRadius + other.fRadius);
        walkable = walkable && (dist > minDist || dist < 0.0001);
    });
    if(!walkable) return false;
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
        if(other.fTeamId != mTeamId) return;
        eVec2d diff = ePointF::vector(mPos, other.fPos);
        const double dist = diff.length();
        if(dist > mNearbyUnits) return;
        diff.normalize();

        const double minDist = mRadius + other.fRadius;
        if(dist < minDist && dist > 0.0001) {
            separation += diff*(minDist - dist);
        }

        const auto relPos = ePointF::vector(other.fPos, mPos);
        auto normRelPos = relPos;
        normRelPos.normalize();
        const auto relVel = mVel - other.fVel;
        if(eVec2d::dot(relPos, relVel) < 0) {
            avoid -= normRelPos;
        }
    });

    auto moveDir = desiredDir*1.0 +
                   separation*1.5 +
                   avoid*0.35 +
                   eVec2d::random()*mMoveRandom;
    if(moveDir.length() > 0) {
        moveDir.normalize();
    }
    mVel = moveDir*mSpeed;
    const double progress = eVec2d::dot(mVel, desiredDir);
    if(progress < 0.1) {
        mStuckTimer += by;
    } else {
        mStuckTimer = 0.;
    }
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

    if(mStuckTimer > 1.) {
        stopMoving();
    }

    return true;
}

void eMovementHandler::stopMoving() {
    mStuckTimer = 0.;
    mGoal.stopMoving();
}

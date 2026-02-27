#include "eSlayerHelpers/emovementgoal.h"

#include "eSlayerHelpers/epathsmoother.h"

#include <cstdio>

bool eMovementGoal::increment(const ePointF& from,
                              ePointF& to,
                              const double dist) {
    eVec2d vec;
    switch(mType) {
    case eMovementGoalType::none:
        return false;
    case eMovementGoalType::dir: {
        vec = eVec2d{mDir.fX - from.fX,
                     mDir.fY - from.fY};
        if(vec.length() > dist) {
            vec.normalize(dist);
        }
    } break;
    case eMovementGoalType::path: {
        if(mPath.empty()) {
            stopMoving();
            return false;
        }
        const auto iPos = from.floor();
        const int margin = 2;
        const int dim = 2*margin + 1;
        ePathFinderMap map(iPos.fX - margin, iPos.fY - margin, dim, dim);
        for(int x = iPos.fX - margin; x <= iPos.fX + margin; x++) {
            for(int y = iPos.fY - margin; y <= iPos.fY + margin; y++) {
                map.set({x, y}, mWalkable(x, y));
            }
        }
        int skipNodes;
        vec = ePathSmoother::moveDir(
            mPath, map, from, 1., dist, skipNodes);
        for(int i = 0; i < skipNodes && !mPath.empty(); i++) {
            mPath.erase(mPath.begin());
        }
    } break;
    }
    if(vec.length() < 0.0001) {
        stopMoving();
        return false;
    }
    to = ePointF{from.fX + vec.x,
                 from.fY + vec.y};

    return true;
}

void eMovementGoal::setWalkable(const eWalkable& w) {
    mWalkable = w;
}

void eMovementGoal::moveInDir(const ePointF& dir) {
    mType = eMovementGoalType::dir;
    mDir = dir;
}

void eMovementGoal::moveOnPath(const ePathFinderPath& path) {
    mType = eMovementGoalType::path;
    mPath = path;
}

void eMovementGoal::stopMoving() {
    mType = eMovementGoalType::none;
}

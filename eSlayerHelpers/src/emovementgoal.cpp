#include "eSlayerHelpers/emovementgoal.h"

#include <cstdio>

void eMovementGoalData::moveInDir(const ePointF& dir) {
    mType = eMovementGoalType::dir;
    mDir = dir;
}

void eMovementGoalData::moveOnPath(const ePathFinderPath& path) {
    mType = eMovementGoalType::path;
    mPath = path;
}

void eMovementGoalData::stopMoving() {
    mType = eMovementGoalType::none;
    mPath.clear();
}

bool eMovementGoal::goal(ePointF& to) {
    switch(mType) {
    case eMovementGoalType::none:
        return false;
    case eMovementGoalType::dir: {
        to = mDir;
        return true;
    } break;
    case eMovementGoalType::path: {
        if(mPath.empty()) return false;
        to = mPath.front().fDst;
        return true;
    } break;
    }
    return false;
}

bool eMovementGoal::nextWaypoint() {
    if(mPath.empty()) return false;
    mPath.pop_front();
    return true;
}

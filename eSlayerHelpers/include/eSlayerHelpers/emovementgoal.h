#ifndef EMOVEMENTGOAL_H
#define EMOVEMENTGOAL_H

#include "eslayerhelpersexport.h"

#include "epathfinderpath.h"

#include <functional>

using eWalkable = std::function<bool(const int x, const int y)>;

enum class eMovementGoalType {
    none, dir, path
};

class ESLAYERHELPERS_API eMovementGoalData {
public:
    eMovementGoalType type() const { return mType; }
    bool moving() const { return mType != eMovementGoalType::none; }

    void moveInDir(const ePointF& dir);
    void moveOnPath(const ePathFinderPath& path);
    void stopMoving();
protected:
    eMovementGoalType mType = eMovementGoalType::none;

    ePointF mDir;
    ePathFinderPath mPath;
};

class ESLAYERHELPERS_API eMovementGoal : public eMovementGoalData {
public:
    bool goal(ePointF& to);
    bool nextWaypoint();
};

#endif // EMOVEMENTGOAL_H

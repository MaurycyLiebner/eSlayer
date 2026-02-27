#ifndef EMOVEMENTGOAL_H
#define EMOVEMENTGOAL_H

#include "eslayerhelpersexport.h"

#include "epathfinderpath.h"

#include <functional>

using eWalkable = std::function<bool(const int x, const int y)>;

class ESLAYERHELPERS_API eMovementGoal {
public:
    enum class eMovementGoalType {
        none, dir, path
    };

    eMovementGoalType type() const { return mType; }
    bool moving() const { return mType != eMovementGoalType::none; }

    bool increment(const ePointF& from,
                   ePointF& to,
                   const double dist);

    void setWalkable(const eWalkable& w);

    void moveInDir(const ePointF& dir);
    void moveOnPath(const ePathFinderPath& path);
    void stopMoving();
private:
    eMovementGoalType mType = eMovementGoalType::none;

    ePointF mDir;
    ePathFinderPath mPath;

    eWalkable mWalkable;
};

#endif // EMOVEMENTGOAL_H

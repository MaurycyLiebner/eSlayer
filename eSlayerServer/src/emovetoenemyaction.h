#ifndef EMOVETOENEMYACTION_H
#define EMOVETOENEMYACTION_H

#include "eunitaction.h"

#include <eSlayerHelpers/epoint.h>

class eMoveToEnemyAction : public eUnitAction {
public:
    eMoveToEnemyAction(eServerUnit& unit,
                       eServerArea& area,
                       const int runAnimId,
                       const int walkAnimId,
                       const int walkReadyAnimId,
                       const float maxDist = 10.f);

    void increment(const float by) override;

    void setMaxDist(const float maxDist);
    bool findNewTarget();
private:
    void setTarget(const eServerUnit& u);

    const int mRunAnimId;
    const int mWalkAnimId;
    const int mWalkReadyAnimId;

    int mTargetId = -1;
    float mMaxDist = 10.f;
    ePointF mTargetPos{0.f, 0.f};
};

#endif // EMOVETOENEMYACTION_H

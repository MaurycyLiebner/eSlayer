#ifndef EMOVETOENEMYACTION_H
#define EMOVETOENEMYACTION_H

#include "../emovetotarget.h"

class eMoveToEnemyAction : public eMoveToTarget {
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
    float mMaxDist = 10.f;
};

#endif // EMOVETOENEMYACTION_H

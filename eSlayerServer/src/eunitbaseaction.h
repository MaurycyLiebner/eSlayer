#ifndef EUNITBASEACTION_H
#define EUNITBASEACTION_H

#include "ecomplexaction.h"

#include <eSlayerHelpers/epoint.h>

class eUnitBaseAction : public eComplexAction {
public:
    eUnitBaseAction(eServerUnit& unit,
                    eServerArea& area);

    void increment(const float by) override;
protected:
    void decide() override;
    bool moveToEnemy(const float maxDist);
    void wait(const float time);
    bool lookForAttackTarget();
private:
    int mRunAnimId;
    int mWalkAnimId;
    int mWalkReadyAnimId;
    int mStandAnimId;
    int mStandReadyAnimId;

    constexpr static const float sAttackCounterMax = 25.f;
    float mAttackCounter = eRand::randF(0.f, sAttackCounterMax);
    bool mAttacking = false;
};

#endif // EUNITBASEACTION_H

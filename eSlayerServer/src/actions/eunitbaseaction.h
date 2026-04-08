#ifndef EUNITBASEACTION_H
#define EUNITBASEACTION_H

#include "ecomplexaction.h"

#include <eSlayerHelpers/epoint.h>

enum class eUnitStrategy {
    attack, move
};

class eUnitBaseAction : public eComplexAction {
public:
    eUnitBaseAction(eServerUnit& unit,
                    eServerArea& area);

    void increment(const float by) override;
protected:
    void decide() override;

    void setStrategy(const eUnitStrategy s)
    { mStrategy = s; }

    bool attacking() const { return mAttacking; }

    bool checkForAttackIncrement(const float by);
    bool moveToEnemy(const float maxDist);
    void wait(const float time);
    void walkAround(const float time);
    bool lookForAttackTarget();

    void setAttackDist(const float dist) { mAttackDist = dist; }

    int mRunAnimId;
    int mWalkAnimId;
    int mWalkReadyAnimId;
    int mStandAnimId;
    int mStandReadyAnimId;
private:
    float mAttackDist = 10.f;
    eUnitStrategy mStrategy = eUnitStrategy::attack;

    constexpr static const float sAttackCounterMax = 25.f;
    float mAttackCounter = eRand::randF(0.f, sAttackCounterMax);
    bool mAttacking = false;
};

#endif // EUNITBASEACTION_H

#ifndef EUNITBASEACTION_H
#define EUNITBASEACTION_H

#include "ecomplexaction.h"

#include <eSlayerHelpers/epoint.h>

class eUnitBaseAction : public eComplexAction {
public:
    using eComplexAction::eComplexAction;

    void increment(const float by) override;
    void decide() override;
private:
    bool lookForAttackTarget();

    constexpr static const float sAttackCounterMax = 25.f;
    float mAttackCounter = eRand::randF(0.f, sAttackCounterMax);
    bool mAttacking = false;
};

#endif // EUNITBASEACTION_H

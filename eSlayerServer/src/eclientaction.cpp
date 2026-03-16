#include "eclientaction.h"

#include "eserverunit.h"

void eClientAction::increment(const float by) {
    mUnit.fActionTime -= by;
    if(mUnit.fHealth <= 0) {
        mAttackTarget = eAttackData();
    } else {
        eComplexAction::increment(by);
    }
}

void eClientAction::decide() {
    if(mAttackTarget.fType != eAttackTargetType::none) {
        const bool r = eComplexAction::attack(mAttackTarget);
        if(!r) mAttackTarget = eAttackData();
    }
}

void eClientAction::attack(const eAttackData& target) {
    mAttackTarget = target;
}

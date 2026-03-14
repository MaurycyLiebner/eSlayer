#include "eclientaction.h"

#include "eserverunit.h"

void eClientAction::increment(const double by) {
    mUnit.fActionTime -= by;
    if(mUnit.fHealth <= 0) {
        mAttackTarget = nullptr;
    } else {
        eComplexAction::increment(by);
    }
}

void eClientAction::decide() {
    if(mAttackTarget) {
        const bool r = eComplexAction::attack(*mAttackTarget);
        if(!r) mAttackTarget = nullptr;
    }
}

void eClientAction::attack(const std::shared_ptr<eServerUnit>& target) {
    mAttackTarget = target;
}

#include "eclientaction.h"

#include "../eserverunit.h"

void eClientAction::increment(const float by) {
    if(mUnit.fHealth <= 0) {
        mAttackTarget = eAttackData();
    } else {
        eComplexAction::increment(by);
    }
}

void eClientAction::decide() {
    if(mAttackTarget.fType != eAttackTargetType::none) {
        const auto r = eComplexAction::attack(mAttackTarget);
        const bool failed = r == eAttackResult::failed;
        if(failed || mStopPlanned) mAttackTarget = eAttackData();
    }
    mStopPlanned = false;
}

void eClientAction::attack(const eAttackData& target) {
    if(mAttackTarget.fType != eAttackTargetType::none &&
       target.fType == eAttackTargetType::none) {
        const bool stop = hasChild();
        if(stop) {
            mAttackTarget = target;
        } else {
            mStopPlanned = true;
        }
    } else {
        mAttackTarget = target;
    }
}

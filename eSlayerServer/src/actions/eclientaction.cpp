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
        const bool r = eComplexAction::attack(mAttackTarget);
        if(!r || mStopPlanned) mAttackTarget = eAttackData();
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

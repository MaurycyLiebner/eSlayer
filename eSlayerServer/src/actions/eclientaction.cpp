#include "eclientaction.h"

#include "../eserverunit.h"

void eClientAction::increment(const float by) {
    if(mUnit.fHealth <= 0) {
        mAttackTarget = eAttackData();
    }
    eComplexAction::increment(by);
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
    if(target.fType == eAttackTargetType::position) {
        const auto vec = ePointF::vector(target.fPos, mUnit.fPos);
        const float angle = vec.angle();
        mUnit.setAngle(angle);
    }
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

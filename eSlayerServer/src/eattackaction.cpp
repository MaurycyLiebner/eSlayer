#include "eattackaction.h"

void eAttackAction::increment(const double by) {
    mDuration -= by;
    mActionTime -= by;
    if(mAction && mActionTime <= 0) {
        mAction();
        mAction = nullptr;
    }
    if(mDuration <= 0) finishAction();
}

void eAttackAction::setDuration(const double d) {
    mDuration = d;
}

void eAttackAction::setAction(const double time, const eAction& a) {
    mActionTime = time;
    mAction = a;
}

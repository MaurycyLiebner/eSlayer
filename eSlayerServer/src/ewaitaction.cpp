#include "ewaitaction.h"

void eWaitAction::increment(const double by) {
    mRemTime -= by;
    if(mRemTime <= 0) finishAction();
}

void eWaitAction::setRemTime(const double t) {
    mRemTime = t;
}

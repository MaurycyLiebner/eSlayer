#include "eattackaction.h"

void eAttackAction::increment(const double by) {
    mAttackTime -= by;
    if(mAttackTime <= 0) finishAction();
}

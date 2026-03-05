#include "eunit.h"

void eUnit::intialize(const eWalkable& w,
                      const int charId) {
    mCharId = charId;
    // mHandler.intialize(w, o, charId);
}

void eUnit::increment(const double by) {
    const bool r = mHandler.increment(by);
    if(r) {
        mModel.setAnimation(1);
        mModel.setAngle(mHandler.angle());
    } else {
        mHandler.stopMoving();
        mModel.setAnimation(0);
    }
}

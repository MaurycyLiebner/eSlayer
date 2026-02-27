#include "eunit.h"

void eUnit::intialize(const eWalkable& w,
                      const eObsticle& o,
                      const int charId) {
    mCharId = charId;
    mHandler.intialize(w, o, charId);
}

void eUnit::pushPlanned(const std::queue<eIdPointF>& planned) {
    mHandler.pushPlanned(planned);
}

void eUnit::increment(const double by) {
    const bool r = mHandler.increment(by);
    if(r) {
        mModel.setAnimation(1);
        mModel.setAngle(mHandler.angle());
    } else {
        mModel.setAnimation(0);
    }
}

#include "eserverunit.h"

#include "eunitaction.h"

#include "eSlayerHelpers/emovementhandler.h"

int eServerUnit::sNextCharId = 0;

eServerUnit::eServerUnit(const eCharData& data)
    : mData(data) {}

void eServerUnit::increment(const double by) {
    if(mAction) mAction->increment(by);
    const bool r = mHandler.increment(1.);
    if(r) {
        const auto newPos = mHandler.pos();
        fVel = ePointF::vector(newPos, fPos);
        fAngle = fVel.angle();
        fPos = newPos;
    } else {
        mHandler.stopMoving();
        fVel = eVec2d{0., 0.};
    }
}

void eServerUnit::setAction(const std::shared_ptr<eUnitAction>& a) {
    mAction = a;
}

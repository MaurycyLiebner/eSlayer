#include "eserverunit.h"

#include "ecomplexaction.h"

#include "eSlayerHelpers/emovementhandler.h"

int eServerUnit::sNextCharId = 0;

eServerUnit::eServerUnit(const eCharData& data)
    : mData(data) {}

double eServerUnit::sHitChance(
    const eServerUnit& hit, const eServerUnit& by) {
    const double alvl = by.level();
    const double dlvl = hit.level();
    const double ar = by.attackRating();
    const double dr = hit.defense();
    return std::clamp(2.*alvl/(alvl + dlvl)*ar/(ar + dr), 0.05, 0.95);
}

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

void eServerUnit::setAction(const std::shared_ptr<eComplexAction>& a) {
    mAction = a;
}

void eServerUnit::setChildAction(const std::shared_ptr<eUnitAction>& a) {
    if(mAction) mAction->setChild(a);
}

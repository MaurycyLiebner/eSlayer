#include "eserverunit.h"

#include "ecomplexaction.h"

#include "eSlayerHelpers/emovementhandler.h"

int eServerUnit::sNextCharId = 0;

eServerUnit::eServerUnit(const eCharData& data)
    : mData(data) {}

float eServerUnit::sHitChance(
    const eServerUnit& hit, const eServerUnit& by) {
    const float alvl = by.level();
    const float dlvl = hit.level();
    const float ar = by.attackRating();
    const float dr = hit.defense();
    return std::clamp(2.f*alvl/(alvl + dlvl)*ar/(ar + dr), 0.05f, 0.95f);
}

void eServerUnit::increment(const float by) {
    if(mAction) mAction->increment(by);
    const bool r = mHandler.increment(1.f);
    if(r) {
        const auto newPos = mHandler.pos();
        fVel = ePointF::vector(newPos, fPos);
        fAngle = fVel.angle();
        fPos = newPos;
    } else {
        mHandler.stopMoving();
        fVel = eVec2f{0.f, 0.f};
    }
}

void eServerUnit::setAction(const std::shared_ptr<eComplexAction>& a) {
    mAction = a;
}

void eServerUnit::setChildAction(const std::shared_ptr<eUnitAction>& a) {
    if(mAction) mAction->setChild(a);
}

#include "efolloweraction.h"

#include "../eserverarea.h"
#include "emovetotarget.h"

eFollowerAction::eFollowerAction(
    eServerUnit& unit, eServerArea& area,
    const std::shared_ptr<eServerUnit>& follow) :
    eUnitBaseAction(unit, area),
    mFollow(follow) {
    setAttackDist(5.f);
}

void eFollowerAction::increment(const float by) {
    if(const auto f = mFollow.lock()) {
        if(mUnit.fHealth > 0) {
            const float dist = ePointF::distance(f->fPos, mUnit.fPos);
            if(dist > 15.f) {
                teleportTo(*f);
            } else if(mMoving) {
            } else {
                const bool attacking = eUnitBaseAction::attacking();
                setStrategy(eUnitStrategy::attack);
                if(dist > 7.f) {
                    setStrategy(eUnitStrategy::move);
                    moveTo(*f);
                } else if(dist > 3.f && !attacking) {
                    moveTo(*f);
                }
            }
        }
    } else {
        mArea.planRemoveUnit(mUnit.fCharId);
    }
    eUnitBaseAction::increment(by);
}

void eFollowerAction::decide() {
    mMoving = false;
    eUnitBaseAction::decide();
}

void eFollowerAction::teleportTo(eServerUnit& follow) {
    ePointF pos;
    const bool r = mArea.findPlaceForUnit(follow.fPos, pos);
    if(!r) return;
    mUnit.setPosition(pos);
    setChild(nullptr);
    mUnit.setBlockingActionTime(0.f);
}

void eFollowerAction::moveTo(eServerUnit& follow) {
    mMoving = true;
    const auto move = std::make_shared<eMoveToTarget>(
        mUnit, mArea, mRunAnimId,
        mWalkAnimId, mWalkReadyAnimId);
    move->setTarget({{follow.fCharId, follow.fPos}}, false);
    move->setArriveDist(1.f);
    setChild(move);
}

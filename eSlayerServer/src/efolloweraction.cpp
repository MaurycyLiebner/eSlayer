#include "efolloweraction.h"

#include "eserverarea.h"
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
        const float dist = ePointF::distance(f->fPos, mUnit.fPos);
        if(dist > 15.f) {
            teleportTo(*f);
        } else if(mMoving) {
        } else {
            const bool attacking = eUnitBaseAction::attacking();
            setStrategy(eUnitStrategy::attack);
            if(dist > 10.f) {
                setStrategy(eUnitStrategy::move);
                moveTo(*f);
            } else if(dist > 5.f && !attacking) {
                moveTo(*f);
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
    const auto pos = mArea.emptyPlaceNear(follow.fPos);
    auto& handler = mUnit.movementHandler();
    handler.setPos(pos);
    mUnit.fPos = pos;
    setChild(nullptr);
}

void eFollowerAction::moveTo(eServerUnit& follow) {
    mMoving = true;
    const auto move = std::make_shared<eMoveToTarget>(
        mUnit, mArea, mRunAnimId,
        mWalkAnimId, mWalkReadyAnimId);
    move->setTarget(follow);
    move->setArriveDist(1.f);
    setChild(move);
}

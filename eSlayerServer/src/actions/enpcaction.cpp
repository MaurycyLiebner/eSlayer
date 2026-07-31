#include "enpcaction.h"

#include "../eserverunit.h"
#include "../eserverarea.h"
#include "ewaitaction.h"
#include "ewalkaroundaction.h"
#include "emovetotarget.h"

eNPCAction::eNPCAction(eServerUnit& unit,
                       eServerArea& area) :
    eComplexAction(unit, area) {
    const auto& data = mUnit.data();
    mWalkAnimId = data.animId("walk");
    mStandAnimId = data.animId("stand");
    mMainPos = mUnit.fPos;
}

void eNPCAction::increment(const float by) {
    if(mMoving) {
        const bool r = mArea.iterateOverUnitsClamped(
            mUnit.fPos, 1.f, [](
                const std::shared_ptr<eServerUnit>& u) {
            return u->isSlayer();
        });
        if(r) wait(100.f);
    }
    eComplexAction::increment(by);
}

void eNPCAction::decide() {
    if(mWalkAnimId < 0) return wait(1000.f);
    if(eRand::randChance(0.1f)) {
        goBack();
    } else if(eRand::randChance(0.5f)) {
        walkAround(100.f);
    } else {
        wait(100.f);
    }
}

void eNPCAction::wait(const float time) {
    mMoving = false;
    const auto wait = eWaitAction::sCreateStand(
        mUnit, mArea, mStandAnimId, -1, time);
    setChild(wait);
}

void eNPCAction::walkAround(const float time) {
    mMoving = true;
    const auto walkAround = eWalkAroundAction::sCreate(
        mUnit, mArea, mWalkAnimId, -1, time);
    walkAround->setMaxDist(1.f);
    walkAround->setCenterPos(mMainPos);
    setChild(walkAround);
}

void eNPCAction::goBack() {
    mMoving = true;
    const auto goBack = std::make_shared<eMoveToTarget>(
        mUnit, mArea, -1, mWalkAnimId, -1);
    const eUnitTarget target(0, mMainPos);
    goBack->setTarget({target}, false);
    setChild(goBack);
}
#include "enpcaction.h"

#include "../eserverunit.h"
#include "../eserverarea.h"
#include "ewaitaction.h"
#include "ewalkaroundaction.h"
#include "emovetotarget.h"

#include <eSlayerHelpers/eportals.h>

eNPCAction::eNPCAction(eServerUnit& unit,
                       eServerArea& area) :
    eComplexAction(unit, area) {
    const auto& data = mUnit.data();
    mWalkAnimId = data.animId("walk");
    mStandAnimId = data.animId("stand");
    mWoundedAnimId = data.animId("wounded");
    mMainPos = mUnit.fPos;
}

void eNPCAction::increment(const float by) {
    if(mInterruptable) {
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
    switch(mStage) {
    case eNPCStage::none: {
        if(mWalkAnimId < 0) return wait(1000.f);
        if(eRand::randChance(0.1f)) {
            goBack();
        } else if(eRand::randChance(0.5f)) {
            walkAround(100.f);
        } else {
            wait(100.f);
        }
    } break;
    case eNPCStage::wounded: {
        layWounded();
    } break;
    case eNPCStage::standCured: {
        leave();
    } break;
    case eNPCStage::leave: {
        ePortal::removePortal(mPortalId);
        mPortalId = 0;
        mArea.hideUnit(mUnit);
    } break;
    }
}

void eNPCAction::layWounded() {
    mInterruptable = false;
    mStage = eNPCStage::wounded;
    const auto wait = eWaitAction::sCreate(
        mUnit, mArea, mWoundedAnimId, true);
    wait->setDuration(1000.f);
    setChild(wait);
}

void eNPCAction::cure() {
    mInterruptable = false;
    mStage = eNPCStage::standCured;
    wait(50.f);
}

void eNPCAction::wait(const float time) {
    mInterruptable = false;
    const auto wait = eWaitAction::sCreateStand(
        mUnit, mArea, mStandAnimId, -1, time);
    setChild(wait);
}

void eNPCAction::walkAround(const float time) {
    mInterruptable = true;
    const auto walkAround = eWalkAroundAction::sCreate(
        mUnit, mArea, mWalkAnimId, -1, time);
    walkAround->setMaxDist(1.f);
    walkAround->setCenterPos(mMainPos);
    setChild(walkAround);
}

void eNPCAction::goBack() {
    mInterruptable = true;
    const auto goBack = std::make_shared<eMoveToTarget>(
        mUnit, mArea, -1, mWalkAnimId, -1);
    const eUnitTarget target(0, mMainPos);
    goBack->setTarget({target}, false);
    setChild(goBack);
}

void eNPCAction::leave() {
    mInterruptable = false;
    mStage = eNPCStage::leave;

    mPortalId = 0;
    eAreaIds areaIds;
    mPortalPos = mUnit.fPos + eVec2f{1.f, -1.f,};
    const bool r = mArea.spawnPortal(
        mPortalPos, mPortalId, areaIds);
    if(!r) {
        mPortalId = 0;
        mArea.hideUnit(mUnit);
        return;
    }
    {
        ePortal p;
        p.fCreator = mUnit.fCharId;
        p.fOutdoorArea = areaIds;
        p.fOutdoorPortalId = mPortalId;
        p.fOutdoorPos = mPortalPos;
        ePortal::addPortal(p);
    }

    const auto move = std::make_shared<eMoveToTarget>(
        mUnit, mArea, -1, mWalkAnimId, -1);
    const eUnitTarget target(0, mPortalPos);
    move->setArriveDist(0.f);
    move->setTarget({target}, false);
    setChild(move);
}
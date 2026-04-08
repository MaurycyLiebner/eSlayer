#include "eunitbaseaction.h"

#include "emovetoenemyaction.h"
#include "../eserverarea.h"
#include "ewaitaction.h"
#include "ewalkaroundaction.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/erand.h>

eUnitBaseAction::eUnitBaseAction(eServerUnit& unit,
                                 eServerArea& area) :
    eComplexAction(unit, area) {
    const auto& data = mUnit.data();
    mRunAnimId = data.animId("run");
    mWalkAnimId = data.animId("walk");
    mWalkReadyAnimId = data.animId("walkReady");
    mStandAnimId = data.animId("stand");
    mStandReadyAnimId = data.animId("standReady");
}

void eUnitBaseAction::increment(const float by) {
    if(mUnit.fHealth > 0 && mStrategy == eUnitStrategy::attack) {
        checkForAttackIncrement(by);
    }
    eComplexAction::increment(by);
}

bool eUnitBaseAction::checkForAttackIncrement(const float by) {
    if(mAttacking) return true;
    mAttackCounter += by;
    if(mAttackCounter >= sAttackCounterMax) {
        mAttackCounter = 0.f;
        return lookForAttackTarget();
    }
    return false;
}

void eUnitBaseAction::decide() {
    if(mUnit.fHealth <= 0) {
        mArea.planRemoveUnit(mUnit.fCharId);
        return;
    }
    mAttacking = false;
    mAttackCounter = 0.f;
    if(mStrategy == eUnitStrategy::attack) {
        {
            const bool r = lookForAttackTarget();
            if(r) return;
        }
        {
            const bool r = moveToEnemy(mAttackDist);
            if(r) return;
        }
    }
    if(eRand::rand() % 2) {
        walkAround(200.f);
    } else {
        wait(100.f);
    }
}

bool eUnitBaseAction::moveToEnemy(const float maxDist) {
    const auto move = std::make_shared<eMoveToEnemyAction>(
        mUnit, mArea, mRunAnimId,
        mWalkAnimId, mWalkReadyAnimId,
        maxDist);
    setChild(move);
    const bool r = move->findNewTarget();
    return r;
}

void eUnitBaseAction::wait(const float time) {
    const auto wait = eWaitAction::sCreateStand(
        mUnit, mArea, mStandAnimId, mStandReadyAnimId, time);
    setChild(wait);
}

void eUnitBaseAction::walkAround(const float time) {
    const auto walkAround = eWalkAroundAction::sCreate(
        mUnit, mArea, mWalkAnimId, mWalkReadyAnimId, time);
    setChild(walkAround);
}

bool eUnitBaseAction::lookForAttackTarget() {
    bool attack = false;
    int maxRangeSchoice;
    const auto& stats = mUnit.stats();
    const float maxRange = stats.maxRangeSkill(
        maxRangeSchoice, 1.f, mUnit.fRadius);
    auto readySkills = mUnit.readySkills();
    const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
        if(u->fHealth <= 0) return false;
        if(mUnit.fTeamId == u->fTeamId) return false;
        const float dist = ePointF::distance(mUnit.fPos, u->fPos);
        int schoice;
        const bool r = stats.attackRangeSkill(
            dist, schoice,
            u->fRadius, mUnit.fRadius,
            readySkills);
        if(r) {
            const eAttackData data(u->fCharId, schoice);
            attack = eComplexAction::attack(data);
            return attack;
        }
        return false;
    };
    mArea.iterateOverUnits(mUnit.fPos, maxRange, iter);
    mAttacking = attack;
    return attack;
}

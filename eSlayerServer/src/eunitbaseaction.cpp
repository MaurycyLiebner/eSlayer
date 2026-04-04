#include "eunitbaseaction.h"

#include "emovetoenemyaction.h"
#include "eserverarea.h"
#include "ewaitaction.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/erand.h>

void eUnitBaseAction::increment(const float by) {
    if(mAttacking) return eComplexAction::increment(by);
    mAttackCounter += by;
    if(mAttackCounter >= sAttackCounterMax) {
        mAttackCounter = 0.f;
        lookForAttackTarget();
    }
    eComplexAction::increment(by);
}

void eUnitBaseAction::decide() {
    if(mUnit.fHealth <= 0) {
        mArea.removeUnit(mUnit.fCharId);
        return;
    }
    mAttacking = false;
    const bool r = lookForAttackTarget();
    if(r) return;
    if(eRand::rand() % 2) {
        const auto move = std::make_shared<eMoveToEnemyAction>(mUnit, mArea);
        setChild(move);
    } else {
        const auto wait = eWaitAction::sCreateStand(mUnit, mArea, 100.f);
        setChild(wait);
    }
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

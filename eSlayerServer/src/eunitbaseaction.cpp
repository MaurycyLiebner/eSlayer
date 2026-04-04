#include "eunitbaseaction.h"

#include "emovetoenemyaction.h"
#include "eserverarea.h"
#include "ewaitaction.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/erand.h>

void eUnitBaseAction::decide() {
    bool attack = false;
    int maxRangeSchoice;
    const auto& stats = mUnit.stats();
    const float maxRange = stats.maxRangeSkill(
        maxRangeSchoice, 1.f, mUnit.fRadius);
    const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
        if(u->fHealth <= 0) return false;
        if(mUnit.fTeamId == u->fTeamId) return false;
        const float dist = ePointF::distance(mUnit.fPos, u->fPos);
        int schoice;
        const bool r = stats.attackRangeSkill(dist, schoice, u->fRadius, mUnit.fRadius);
        if(r) {
            const eAttackData data(u->fCharId, schoice);
            attack = eComplexAction::attack(data);
            return attack;
        }
        return false;
    };
    mArea.iterateOverUnits(mUnit.fPos, maxRange, iter);
    if(attack) return;
    if(eRand::rand() % 2) {
        const auto move = std::make_shared<eMoveToEnemyAction>(mUnit, mArea);
        setChild(move);
    } else {
        const auto wait = eWaitAction::sCreateStand(mUnit, mArea, 100.f);
        setChild(wait);
    }
}

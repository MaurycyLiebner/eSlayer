#include "eunitbaseaction.h"

#include "emovetoenemyaction.h"
#include "eserverarea.h"
#include "ewaitaction.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/erand.h>

void eUnitBaseAction::decide() {
    bool attack = false;
    const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
        if(mUnit.fTeamId == u->fTeamId) return false;
        attack = meeleAttack(*u, eSkillChoice::left, eWeaponChoice::left);
        return attack;
    };
    mArea.iterateOverUnits(mUnit.fPos, 1.f, iter);
    if(attack) return;
    if(eRand::rand() % 2) {
        const auto move = std::make_shared<eMoveToEnemyAction>(mUnit, mArea);
        setChild(move);
    } else {
        const auto wait = eWaitAction::sCreateStand(mUnit, mArea, 100.f);
        setChild(wait);
    }
}

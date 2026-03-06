#include "eunitbaseaction.h"

#include "emovetoenemyaction.h"
#include "eattackaction.h"
#include "ewaitaction.h"
#include "eserverarea.h"

#include <eSlayerHelpers/erand.h>

void eUnitBaseAction::decide() {
    for(const auto& unit : mArea.units()) {
        if(mUnit.fTeamId == unit->fTeamId) continue;
        const double dist = ePointF::distance(mUnit.fPos, unit->fPos);
        if(dist < 0.5*(mUnit.fRadius + unit->fRadius)) {
            attack(*unit);
            return;
        }
    }
    if(eRand::rand() % 2) {
        const auto move = std::make_shared<eMoveToEnemyAction>(mUnit, mArea);
        setChild(move);
    } else {
        const auto wait = std::make_shared<eWaitAction>(mUnit, mArea);
        wait->setRemTime(100.);
        setChild(wait);
        mUnit.fAnim = 0;
        mUnit.fAnimId++;
    }
}

void eUnitBaseAction::attack(const eServerUnit& u) {
    mUnit.fAnim = 2 + (eRand::rand() % 2);
    mUnit.fAnimId++;
    const auto dir = ePointF::vector(u.fPos, mUnit.fPos);
    mUnit.fAngle = dir.angle();
    const auto attack = std::make_shared<eAttackAction>(mUnit, mArea);
    setChild(attack);
}

#include "eunitbaseaction.h"

#include "emovetoenemyaction.h"
#include "eserverarea.h"
#include "ewaitaction.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/erand.h>

void eUnitBaseAction::decide() {
    for(const auto& unit : mArea.units()) {
        if(mUnit.fTeamId == unit->fTeamId) continue;
        const bool r = attack(*unit);
        if(r) return;
    }
    if(eRand::rand() % 2) {
        const auto move = std::make_shared<eMoveToEnemyAction>(mUnit, mArea);
        setChild(move);
    } else {
        const auto wait = eWaitAction::sCreateStand(mUnit, mArea, 100.f);
        setChild(wait);
    }
}

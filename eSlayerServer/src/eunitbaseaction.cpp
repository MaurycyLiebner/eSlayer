#include "eunitbaseaction.h"

#include "eserverarea.h"

#include <eSlayerHelpers/erand.h>

void eUnitBaseAction::increment(const double by) {
    if(eRand::rand() % 11 == 10) {
        for(const auto& unit : mArea.units()) {
            if(mUnit.fTeamId == unit->fTeamId) continue;
            const double dist = ePointF::distance(mUnit.fPos, unit->fPos);
            if(dist < 5.) {
                auto& handler = mUnit.movementHandler();
                handler.moveTo(unit->fPos);
                break;
            }
        }
    }
}

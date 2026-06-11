#include "emovementhandler.h"

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eslayers.h>

eMovementHandler::eMovementHandler(
    eUnitData& u,
    ePathFinderMap& map) :
    eMovementHandlerBase(u, map),
    mUnit(u) {}

bool eMovementHandler::increment(const float by) {
    float angle;
    ePointF pos;
    const bool r = eMovementHandlerBase::increment(
        by, angle, pos);
    mUnit.setAngle(angle);
    mUnit.setPosition(pos);
    eSlayers::setPoisition(eSlayers::sThisSlayer, pos);
    return r;
}

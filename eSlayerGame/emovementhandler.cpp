#include "emovementhandler.h"

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eslayers.h>

eMovementHandler::eMovementHandler(eUnitData& u) :
    eMovementHandlerBase(u),
    mUnit(u) {}

bool eMovementHandler::increment(const float by) {
    float angle;
    ePointF pos;
    const bool r = eMovementHandlerBase::increment(
        by, angle, pos);
    mUnit.setAngle(angle);
    mUnit.setPosition(pos);
    return r;
}

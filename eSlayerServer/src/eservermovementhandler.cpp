#include "eservermovementhandler.h"

#include "eserverunit.h"

eServerMovementHandler::eServerMovementHandler(
    eServerUnit& u,
    ePathFinderMap& map) :
    eMovementHandlerBase(u, map),
    mUnit(u) {}

bool eServerMovementHandler::increment(const float by) {
    float angle;
    ePointF pos;
    const bool r = eMovementHandlerBase::increment(by, angle, pos);
    mUnit.setAngle(angle);
    mUnit.setPosition(pos);
    return r;
}

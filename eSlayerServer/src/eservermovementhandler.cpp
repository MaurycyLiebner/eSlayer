#include "eservermovementhandler.h"

#include "eserverunit.h"

#include <eSlayerHelpers/eunitsinfo.h>

eServerMovementHandler::eServerMovementHandler(
    eServerUnit& u,
    ePathFinderMap& map,
    const int unitTypeId) :
    eMovementHandlerBase(u, map),
    mUnit(u) {
    const auto& uinfo = eUnitsInfo::sUnits.get(unitTypeId);
    mRunSpeedBase = uinfo.fRunSpeed;
    mWalkSpeedBase = uinfo.fWalkSpeed;
}

bool eServerMovementHandler::increment(const float by) {
    float angle;
    ePointF pos;
    const bool r = eMovementHandlerBase::increment(by, angle, pos);
    if(r) {
        const auto& stats = mUnit.stats();
        const bool run = mUnit.isRunning();
        const float base = run ? mRunSpeedBase : mWalkSpeedBase;
        const float walkRunSpeed = 1.f + stats.fWalkRun;
        const float speed = base*walkRunSpeed;
        setSpeed(speed);

        const float aspeed = 0.5f*walkRunSpeed;
        mUnit.setAnimSpeed(aspeed);
    }
    mUnit.setAngle(angle);
    mUnit.setPosition(pos);
    return r;
}

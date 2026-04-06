#include "emovetotarget.h"

#include "eserverarea.h"

#include <eSlayerHelpers/echardata.h>

eMoveToTarget::eMoveToTarget(
    eServerUnit& unit,
    eServerArea& area,
    const int runAnimId,
    const int walkAnimId,
    const int walkReadyAnimId) :
    eUnitAction(unit, area),
    mRunAnimId(runAnimId),
    mWalkAnimId(walkAnimId),
    mWalkReadyAnimId(walkReadyAnimId) {}

void eMoveToTarget::increment(const float by) {
    auto& handler = mUnit.movementHandler();
    if(handler.stuckTime() > 10.f) {
        handler.stopMoving();
        return finishAction();
    }
    if(mTargetId != -1) {
        const auto target = mArea.unit(mTargetId);
        if(target) {
            const float dist = ePointF::distance(target->fPos, mUnit.fPos);
            if(dist < mArriveDist + 0.5f*(mUnit.fRadius + target->fRadius)) {
                handler.stopMoving();
                finishAction();
            } else {
                const float targetChange = ePointF::distance(target->fPos, mTargetPos);
                const float dist = ePointF::distance(mTargetPos, mUnit.fPos);
                if(targetChange > 0.5f*dist || !handler.moving()) {
                    setTarget(*target);
                }
            }
        } else {
            mTargetId = -1;
        }
    }
}

void eMoveToTarget::setTarget(const eServerUnit& u) {
    auto& handler = mUnit.movementHandler();
    const bool r = handler.moveInDirectionIfClearPath(u.fPos);
    if(!r) handler.moveTo(u.fPos);
    mTargetId = u.fCharId;
    mTargetPos = u.fPos;
    const bool a = mUnit.aggressive();
    if(mRunAnimId != -1) {
        mUnit.fAnim = mRunAnimId;
    } else {
        mUnit.fAnim = eMovementHandler::sChooseAnim(
            mWalkAnimId, mWalkReadyAnimId, a);
    }
    mUnit.fAnimId++;
}

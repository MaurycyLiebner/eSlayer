#include "emovetoenemyaction.h"

#include "eserverarea.h"

void eMoveToEnemyAction::increment(const double by) {
    auto& handler = mUnit.movementHandler();
    if(handler.stuckTime() > 10.) {
        handler.stopMoving();
        return finishAction();
    }
    if(mTargetId != -1) {
        const auto target = mArea.unit(mTargetId);
        if(target) {
            const double dist = ePointF::distance(target->fPos, mUnit.fPos);
            if(dist < 0.5*(mUnit.fRadius + target->fRadius)) {
                handler.stopMoving();
                finishAction();
            } else {
                const double targetChange = ePointF::distance(target->fPos, mTargetPos);
                const double dist = ePointF::distance(mTargetPos, mUnit.fPos);
                if(targetChange > 0.5*dist || !handler.moving()) {
                    setTarget(*target);
                }
            }
            return;
        } else {
            mTargetId = -1;
        }
    }
    for(const auto& unit : mArea.units()) {
        if(mUnit.fTeamId == unit->fTeamId) continue;
        const double dist = ePointF::distance(mUnit.fPos, unit->fPos);
        if(dist < 5.) {
            setTarget(*unit);
            return;
        }
    }
    finishAction();
}

void eMoveToEnemyAction::setTarget(const eServerUnit& u) {
    auto& handler = mUnit.movementHandler();
    const bool r = handler.moveInDirectionIfClearPath(u.fPos);
    if(!r) handler.moveTo(u.fPos);
    mTargetId = u.fCharId;
    mTargetPos = u.fPos;
    mUnit.fAnim = 1;
    mUnit.fAnimId++;
}

#include "emovetoenemyaction.h"

#include "eserverarea.h"

void eMoveToEnemyAction::increment(const double by) {
    auto& handler = mUnit.movementHandler();
    if(mTargetId != -1) {
        const auto target = mArea.unit(mTargetId);
        if(target) {
            const double dist = ePointF::distance(target->fPos, mUnit.fPos);
            if(dist < 0.5*(mUnit.fRadius + target->fRadius)) {
                mUnit.fAnim = 0;
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
    mUnit.fAnim = 0;
    finishAction();
}

void eMoveToEnemyAction::setTarget(const eServerUnit& u) {
    auto& handler = mUnit.movementHandler();
    handler.moveTo(u.fPos);
    mTargetId = u.fCharId;
    mTargetPos = u.fPos;
    mUnit.fAnim = 1;
}

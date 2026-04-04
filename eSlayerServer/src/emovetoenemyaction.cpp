#include "emovetoenemyaction.h"

#include "eserverarea.h"

#include <eSlayerHelpers/echardata.h>

void eMoveToEnemyAction::increment(const float by) {
    auto& handler = mUnit.movementHandler();
    if(handler.stuckTime() > 10.f) {
        handler.stopMoving();
        return finishAction();
    }
    if(mTargetId != -1) {
        const auto target = mArea.unit(mTargetId);
        if(target) {
            const float dist = ePointF::distance(target->fPos, mUnit.fPos);
            if(dist < 0.5f*(mUnit.fRadius + target->fRadius)) {
                handler.stopMoving();
                finishAction();
            } else {
                const float targetChange = ePointF::distance(target->fPos, mTargetPos);
                const float dist = ePointF::distance(mTargetPos, mUnit.fPos);
                if(targetChange > 0.5f*dist || !handler.moving()) {
                    setTarget(*target);
                }
            }
            return;
        } else {
            mTargetId = -1;
        }
    }

    const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
        if(u->fHealth <= 0) return false;
        if(mUnit.fTeamId == u->fTeamId) return false;
        setTarget(*u);
        return true;
    };
    const bool r = mArea.iterateOverUnits(mUnit.fPos, 10.f, iter);
    if(!r) finishAction();
}

void eMoveToEnemyAction::setTarget(const eServerUnit& u) {
    auto& handler = mUnit.movementHandler();
    const bool r = handler.moveInDirectionIfClearPath(u.fPos);
    if(!r) handler.moveTo(u.fPos);
    mTargetId = u.fCharId;
    mTargetPos = u.fPos;
    const bool a = mUnit.aggressive();
    const auto& data = mUnit.data();
    const int normal = data.animId("walk");
    const int aggressive = data.animId("walkReady");
    mUnit.fAnim = eMovementHandler::sChooseAnim(normal, aggressive, a);
    mUnit.fAnimId++;
}

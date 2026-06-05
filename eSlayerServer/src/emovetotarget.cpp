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
    const bool moving = handler.moving();
    for(const auto& t : mTargets) {
        const auto target = mArea.unit(t.fId);
        if(!target) continue;
        const auto& targetPos = target->fPos;
        const float dist = ePointF::distance(targetPos, mUnit.fPos);
        if(dist < mArriveDist + 0.5f*(mUnit.fRadius + target->fRadius)) {
            handler.stopMoving();
            finishAction();
            mTargets.clear();
            break;
        } else {
            const float targetChange = ePointF::distance(targetPos, t.fPos);
            const float dist = ePointF::distance(targetPos, mUnit.fPos);
            if(targetChange > 0.5f*dist || !moving) {
                std::vector<eUnitTarget> targets;
                for(auto& t : mTargets) {
                    const auto target = mArea.unit(t.fId);
                    if(!target) continue;
                    if(target->fHealth < 0) continue;
                    targets.emplace_back(target->fCharId, target->fPos);
                }
                setTarget(targets, mFoundOnly);
                break;
            }
        }
    }
}

bool eMoveToTarget::setTarget(const std::vector<eUnitTarget>& targets,
                              const bool foundOnly) {
    auto& handler = mUnit.movementHandler();
    bool canMove = false;
    for(const auto& t : targets) {
        const bool r = handler.moveInDirectionIfClearPath(t.fPos);
        if(r) {
            canMove = true;
            break;
        }
    }
    if(!canMove) {
        std::vector<ePointF> tos;
        tos.reserve(targets.size());
        for(const auto& t : targets) {
            tos.emplace_back(t.fPos);
        }
        const bool r = handler.moveTo(tos, foundOnly);
        if(!r && foundOnly) {
            handler.stopMoving();
            finishAction();
            return false;
        }
    }
    mTargets = targets;
    mFoundOnly = foundOnly;
    const bool a = mUnit.aggressive();
    if(mRunAnimId != -1) {
        mUnit.fAnim = mRunAnimId;
    } else {
        mUnit.fAnim = eMovementHandler::sChooseAnim(
            mWalkAnimId, mWalkReadyAnimId, a);
    }
    mUnit.fAnimId.increment(1);
    return true;
}

#include "emovetoenemyaction.h"

#include "../eserverarea.h"
#include "../eserverteams.h"

#include <eSlayerHelpers/echardata.h>

eMoveToEnemyAction::eMoveToEnemyAction(
    eServerUnit& unit,
    eServerArea& area,
    const int runAnimId,
    const int walkAnimId,
    const int walkReadyAnimId,
    const float maxDist) :
    eMoveToTarget(unit, area,
                  runAnimId, walkAnimId,
                  walkReadyAnimId),
    mMaxDist(maxDist) {}

void eMoveToEnemyAction::increment(const float by) {
    if(mTargetId == -1) {
        const bool r = findNewTarget();
        if(!r) return finishAction();
    }
    eMoveToTarget::increment(by);
}

void eMoveToEnemyAction::setMaxDist(const float maxDist) {
    mMaxDist = maxDist;
}

bool eMoveToEnemyAction::findNewTarget() {
    const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
        if(u->fHealth <= 0) return false;
        const eTeamId t1 = u->fTeamId;
        const eTeamId t2 = mUnit.fTeamId;
        if(!eServerTeams::areEnemies(t1, t2)) return false;
        setTarget(*u);
        return true;
    };
    return mArea.iterateOverUnits(mUnit.fPos, mMaxDist, iter);
}

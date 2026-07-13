#include "efleeaction.h"

#include "../eserverunit.h"

eFleeAction::eFleeAction(
    eServerUnit& unit,
    const ePointF& from,
    eServerArea& area,
    const int runAnimId,
    const int walkAnimId,
    const int walkReadyAnimId,
    const float maxDist) :
    eMoveToTarget(unit, area,
                  runAnimId, walkAnimId,
                  walkReadyAnimId) {
    std::vector<eUnitTarget> targets;
    const ePointF& pos = unit.fPos;
    const auto dir = ePointF::vector(pos, from);
    const float range = 45.f;
    const int div = 3;
    for(int i = -div; i <= div; i++) {
        auto vec = dir;
        if(i != 0) vec.rotate(i*range/div);
        vec.normalize(maxDist);
        targets.emplace_back(0, pos + vec);
    }
    setTarget(targets, false);
}

void eFleeAction::increment(const float by) {
    eMoveToTarget::increment(by);
}

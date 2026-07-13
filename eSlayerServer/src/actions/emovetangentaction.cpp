#include "emovetangentaction.h"

#include "../eserverunit.h"

eMoveTangentAction::eMoveTangentAction(
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
    const eVec2f tan1{-dir.y, dir.x};
    const eVec2f tan2{dir.y, -dir.x};
    for(const auto& tan : {tan1, tan2}) {
        const float range = 15.f;
        const int div = 1;
        for(int i = -div; i <= div; i++) {
            auto vec = tan;
            if(i != 0) vec.rotate(i*range/div);
            vec.normalize(maxDist);
            targets.emplace_back(0, pos + vec);
        }
    }
    setTarget(targets, false);
}
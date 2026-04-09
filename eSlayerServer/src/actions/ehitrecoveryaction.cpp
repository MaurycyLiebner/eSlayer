#include "ehitrecoveryaction.h"

#include "../eserverunit.h"

#include <eSlayerHelpers/echardata.h>

int eHitRecoveryAction::sRecoveryFrames(const eServerUnit& u) {
    const auto& data = u.data();
    const int anim = data.hitRecoveryAnimId();
    if(anim == -1) return 0;
    const float fhr = u.fasterHitRecovery();
    const float efhr = std::floor(fhr*120.f/(fhr + 120.f));
    const int hitBase = data.animFrames(anim);
    const float animSpeed = 256.f;
    return int(std::ceil(256.f*hitBase/std::floor(animSpeed*(50.f + efhr)/100.f))) - 1;
}

std::shared_ptr<eHitRecoveryAction> eHitRecoveryAction::sCreate(eServerUnit& unit,
                                                                eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.hitRecoveryAnimId();
    if(anim != -1) {
        const int frames = sRecoveryFrames(unit);
        const auto a = std::make_shared<eHitRecoveryAction>(unit, area);
        a->setup(anim, frames, true, nullptr);
        return a;
    } else {
        return nullptr;
    }
}

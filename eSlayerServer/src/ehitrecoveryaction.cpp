#include "ehitrecoveryaction.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eHitRecoveryAction>
eHitRecoveryAction::sCreate(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.animId("hitRecovery");
    if(anim != -1) {
        const double fhr = unit.fasterHitRecovery();
        const double efhr = std::floor(fhr*120./(fhr + 120.));
        const int hitBase = data.animFrames(anim);
        const double animSpeed = 256.;
        const int frames = int(std::ceil(256.*hitBase/std::floor(animSpeed*(50. + efhr)/100.))) - 1;
        const auto a = std::make_shared<eHitRecoveryAction>(unit, area);
        a->setup(anim, frames, nullptr);
        return a;
    } else {
        return nullptr;
    }
}

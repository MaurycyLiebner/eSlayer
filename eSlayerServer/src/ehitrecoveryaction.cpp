#include "ehitrecoveryaction.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eHitRecoveryAction>
eHitRecoveryAction::sCreate(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.animId("hitRecovery");
    if(anim != -1) {
        const auto a = std::make_shared<eHitRecoveryAction>(unit, area);
        a->setup(anim, nullptr);
        return a;
    } else {
        return nullptr;
    }
}

#include "eblockaction.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eBlockAction>
eBlockAction::sCreate(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.animId("block");
    if(anim != -1) {
        const auto getHit = std::make_shared<eBlockAction>(unit, area);
        getHit->setup(anim, nullptr);
        return getHit;
    } else {
        return nullptr;
    }
}

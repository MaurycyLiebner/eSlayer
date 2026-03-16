#include "ewaitaction.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eWaitAction>
eWaitAction::sCreateStand(eServerUnit& unit, eServerArea& area,
                          const float time) {
    const bool a = unit.aggressive();
    const auto& data = unit.data();
    const int naId = data.animId("stand");
    const int aId = data.animId("standReady");
    int anim;
    if(a) {
        if(aId != -1) {
            anim = aId;
        } else {
            anim = naId;
        }
    } else {
        if(naId != -1) {
            anim = naId;
        } else {
            anim = aId;
        }
    }
    const auto result = sCreate(unit, area, anim);
    if(result) result->setDuration(time);
    return result;
}

std::shared_ptr<eWaitAction>
eWaitAction::sCreateDeath(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.animId("death");
    return sCreate(unit, area, anim);
}

std::shared_ptr<eWaitAction>
eWaitAction::sCreateBody(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.animId("body");
    const auto result = sCreate(unit, area, anim);
    if(result) result->setDuration(std::numeric_limits<float>::max());
    return result;
}

std::shared_ptr<eWaitAction> eWaitAction::sCreate(
    eServerUnit& unit, eServerArea& area,
    const int anim) {
    if(anim != -1) {
        const auto wait = std::make_shared<eWaitAction>(unit, area);
        wait->setup(anim, -1, nullptr);
        return wait;
    } else {
        return nullptr;
    }
}

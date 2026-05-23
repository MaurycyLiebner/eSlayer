#include "ewaitaction.h"

#include "../eserverunit.h"
#include "eexplodeaction.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eWaitAction>
eWaitAction::sCreateStand(eServerUnit& unit, eServerArea& area,
                          const int standId, const int standReadyId,
                          const float time) {
    const bool a = unit.aggressive();
    const auto& data = unit.data();
    const int anim = eMovementHandler::sChooseAnim(
        standId, standReadyId, a);
    const auto result = sCreate(unit, area, anim, false);
    if(result) result->setDuration(time);
    return result;
}

std::shared_ptr<eWaitAction>
eWaitAction::sCreateDeath(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.deathAnimId();
    return sCreate(unit, area, anim, true);
}

std::shared_ptr<eWaitAction>
eWaitAction::sCreateBody(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.bodyAnimId();
    const auto result = sCreate(unit, area, anim, true);
    if(result) result->setDuration(std::numeric_limits<float>::max());
    return result;
}

std::shared_ptr<eWaitAction>
eWaitAction::sCreateExplode(
    const eExplodeType type,
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    int anim = -1;
    switch(type) {
    case eExplodeType::flesh:
        anim = sFleshExplAnim;
        break;
    case eExplodeType::ice:
        anim = sIceExplAnim;
        break;
    }
    return sCreate(unit, area, anim, true);
}

std::shared_ptr<eWaitAction> eWaitAction::sCreate(
    eServerUnit& unit, eServerArea& area,
    const int anim, const bool blocking) {
    auto& h = unit.movementHandler();
    h.stopMoving();
    if(anim != -1) {
        const auto wait = std::make_shared<eWaitAction>(unit, area);
        wait->setup(anim, -1, blocking, nullptr);
        return wait;
    } else {
        return nullptr;
    }
}

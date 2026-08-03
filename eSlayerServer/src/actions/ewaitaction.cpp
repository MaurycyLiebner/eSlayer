#include "ewaitaction.h"

#include "../eserverunit.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/especialanim.h>

std::shared_ptr<eWaitAction>
eWaitAction::sCreateStand(eServerUnit& unit, eServerArea& area,
                          const int standId, const int standReadyId,
                          const float time) {
    const bool a = unit.aggressive();
    const auto& data = unit.data();
    const int anim = eMovementHandlerBase::sChooseAnim(
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
eWaitAction::sCreateRaise(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int anim = data.raiseAnimId();
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
    case eExplodeType::none:
        break;
    }
    return sCreate(unit, area, anim, true);
}

std::shared_ptr<eWaitAction>
eWaitAction::sCreateExplodeBody(
    const eExplodeType type,
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    int anim = -1;
    switch(type) {
    case eExplodeType::flesh:
        anim = sFleshExplBody;
        break;
    case eExplodeType::ice:
        anim = sIceExplBody;
        break;
    case eExplodeType::none:
        break;
    }
    const auto result = sCreate(unit, area, anim, true);
    if(result) result->setDuration(std::numeric_limits<float>::max());
    return result;
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

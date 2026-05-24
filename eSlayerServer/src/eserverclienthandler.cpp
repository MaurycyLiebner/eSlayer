#include "eserverclienthandler.h"

#include "actions/eclientaction.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/eattackdata.h>

eServerClientHandler::eServerClientHandler(const int clientId) :
    mClientId(clientId) {}

bool eServerClientHandler::requestEquipment() {
    return true;
}

bool eServerClientHandler::receiveData(eRequestData& data,
                                       float& resultTime) {
    if(!mArea) return false;
    resultTime = mArea->time();
    mArea->unitsData(mClientId, data.fNewUnits, data.fUpdatedUnits);
    data.fMissiles = mArea->missileData(mClientId);
    data.fNovas = mArea->novaData(mClientId);
    data.fSkillAreas = mArea->skillAreaData(mClientId);
    mArea->itemsData(mClientId, data.fNewItems, data.fRemovedItemIds);
    data.fBodies = mArea->bodies(mClientId);
    mArea->mapPortions(mClientId, data.fMapPortions);
    const auto u = mArea->unit(mClientId);
    data.fMana = u ? std::floor(u->mana()) : 0;
    data.fLevel = u ? std::round(u->level()) : 0;
    data.fExperience = u ? std::round(u->experience()) : 0;
    return true;
}

bool eServerClientHandler::receiveEquipment(eEquipment& data) {
    if(!mArea) return false;
    const auto unit = mArea->unit(mClientId);
    if(!unit) return false;
    data = unit->equipment();
    return true;
}

bool eServerClientHandler::changeState(
    const eUnitData& u) {
    if(!mArea) return false;
    const auto unit = mArea->unit(mClientId);
    if(!unit) return false;
    if(ePointF::distance(unit->fPos, u.fPos) > 0.0001f) {
        unit->fPos = u.fPos;
        unit->setMoving(true);
    } else {
        unit->setMoving(false);
    }
    unit->fAngle = u.fAngle;

    if(unit->fBlockingActionTime <= 0.f) {
        unit->fAnim = u.fAnim;
        unit->fAnimId = u.fAnimId;
        unit->fAnimSpeed = u.fAnimSpeed;
    }
    return true;
}

bool eServerClientHandler::attack(const eAttackData& target) {
    if(!mArea) return false;
    const auto client = mArea->unit(mClientId);
    if(!client) return false;
    switch(target.fType) {
    case eAttackTargetType::character:
    case eAttackTargetType::position: {
        const auto a = client->action();
        if(const auto ca = dynamic_cast<eClientAction*>(a.get())) {
            ca->attack(target);
        }
    } break;
    case eAttackTargetType::none: {
        return false;
    } break;
    }

    return true;
}

bool eServerClientHandler::stopAttack() {
    if(!mArea) return false;
    const auto client = mArea->unit(mClientId);
    if(!client) return false;

    const auto a = client->action();
    if(const auto ca = dynamic_cast<eClientAction*>(a.get())) {
        ca->attack(eAttackData());
    }

    return true;
}

bool eServerClientHandler::respawn() {
    if(!mArea) return false;
    return mArea->respawn(mClientId);
}

bool eServerClientHandler::spawn(eCharacter& c,
                                 eTeamId& teamId,
                                 const eScreenDimensions& screenDims) {
    if(!mArea) return false;
    const auto client = mArea->unit(mClientId);
    if(client) return false;
    mArea->addClient(mClientId, c, teamId, screenDims);
    return true;
}

bool eServerClientHandler::disconnect() {
    if(!mArea) return false;
    return mArea->removeClient(mClientId);
}

bool eServerClientHandler::setSkillId(
    const eSkillChoice schoice,
    const int skillId) {
    if(!mArea) return false;
    const auto client = mArea->unit(mClientId);
    if(!client) return false;
    client->setSkillId(schoice, skillId);
    return true;
}

std::shared_ptr<eObject> eServerClientHandler::triggerObject(
    const int objectId,
    const int tx, const int ty) {
    if(!mArea) return nullptr;
    return mArea->triggerObject(mClientId, objectId, tx, ty);
}

bool eServerClientHandler::triggerDoors(
    const eDoors& doors) {
    if(!mArea) return false;
    return mArea->triggerDoors(mClientId, doors);
}

bool eServerClientHandler::pickupItem(
    const int itemId, const bool drag) {
    if(!mArea) return false;
    return mArea->pickupItem(mClientId, itemId, drag);
}

bool eServerClientHandler::dropItem() {
    if(!mArea) return false;
    mArea->dropItem(mClientId);
    return true;
}

bool eServerClientHandler::rearrangeItems(
    const eEquipment& eq) {
    if(!mArea) return false;
    mArea->rearrangeItems(mClientId, eq);
    return true;
}

bool eServerClientHandler::changeAttributes(
    const eAttributes& attrs) {
    if(!mArea) return false;
    mArea->changeAttributes(mClientId, attrs);
    return true;
}

bool eServerClientHandler::changeSkillLevels(
    const eSkillLevels& skillLevels) {
    if(!mArea) return false;
    mArea->changeSkillLevels(mClientId, skillLevels);
    return true;
}

bool eServerClientHandler::consumePotion(
    const uint32_t itemId) {
    if(!mArea) return false;
    mArea->consumePotion(mClientId, itemId);
    return true;
}

bool eServerClientHandler::pickupBody(
    const uint32_t bodyId) {
    if(!mArea) return false;
    mArea->pickupBody(mClientId, bodyId);
    return true;
}

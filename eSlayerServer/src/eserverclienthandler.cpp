#include "eserverclienthandler.h"

#include "actions/eclientaction.h"

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/eattackdata.h>

eServerClientHandler::eServerClientHandler(
    const uint32_t clientId) :
    mClientId(clientId) {}

bool eServerClientHandler::requestEquipment() {
    return true;
}

bool eServerClientHandler::receiveData(
    eRequestData& data, float& resultTime) {
    if(!mArea) return false;
    resultTime = mArea->time();
    mArea->unitsData(mClientId, data.fNewUnits, data.fUpdatedUnits);
    data.fMissiles = mArea->missileData(mClientId);
    data.fNovas = mArea->novaData(mClientId);
    data.fSkillAreas = mArea->skillAreaData(mClientId);
    mArea->itemsData(mClientId, data.fNewItems, data.fRemovedItemIds);
    const bool update = mArea->updateBoostsAuras(mClientId);
    data.fUpdateBoostsAuras = update;
    if(update) {
        data.fBoosts = mArea->boosts(mClientId);
        data.fAuras = mArea->auras(mClientId);
    }
    mArea->mapPortions(mClientId, data.fMapPortions);
    const auto u = mArea->unit(mClientId);
    data.fMana = u ? std::floor(u->mana()) : 0;
    data.fStamina = u ? std::floor(u->stamina()) : 0;
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

std::shared_ptr<eMap> eServerClientHandler::map() const {
    if(!mArea) return nullptr;
    return mArea->map();
}

bool eServerClientHandler::changeState(
    const eUnitData& u) {
    if(!mArea) return false;
    const auto unit = mArea->unit(mClientId);
    if(!unit) return false;
    if(u.getUpdate(eUnitData::eShift::position)) {
        if(ePointF::distance(unit->fPos, u.fPos) > 0.0001f) {
            unit->setPosition(u.fPos);
            unit->setMoving(true);
        } else {
            unit->setMoving(false);
        }
    }
    if(u.getUpdate(eUnitData::eShift::angle)) {
        unit->setAngle(u.fAngle);
    }

    if(unit->fBlockingActionTime <= 0.f) {
        if(u.getUpdate(eUnitData::eShift::anim)) {
            unit->setAnim(u.fAnim);
        }

        if(u.getUpdate(eUnitData::eShift::animId)) {
            unit->setAnimId(u.fAnimId);
        }

        if(u.getUpdate(eUnitData::eShift::animSpeed)) {
            unit->setAnimSpeed(u.fAnimSpeed);
        }
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

bool eServerClientHandler::respawn(
    uint32_t& bodyId, ePointF& bodyPos) {
    if(!mArea) return false;
    return mArea->respawn(mClientId, bodyId, bodyPos);
}

bool eServerClientHandler::spawn(
    eCharacter& c,
    eTeamId& teamId,
    ePointF& spawnPos,
    std::vector<eBody>& bodies,
    const eScreenDimensions& screenDims) {
    if(!mArea) return false;
    const auto client = mArea->unit(mClientId);
    if(client) return false;
    mArea->addClient(mClientId, c, teamId,
                     spawnPos, bodies, screenDims);
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

bool eServerClientHandler::triggerObject(
    eServerObject& obj) {
    if(!mArea) return false;
    return mArea->triggerObject(mClientId, obj);
}

bool eServerClientHandler::triggerDoors(
    const eServerDoors& doors) {
    if(!mArea) return false;
    return mArea->triggerDoors(mClientId, doors);
}

bool eServerClientHandler::pickupItem(
    const uint32_t itemId, const bool drag,
    eEquipmentAction& action) {
    if(!mArea) return false;
    return mArea->pickupItem(mClientId, itemId, drag, action);
}

bool eServerClientHandler::dropItem() {
    if(!mArea) return false;
    mArea->dropItem(mClientId);
    return true;
}

bool eServerClientHandler::dropGold(
    const uint32_t count) {
    if(!mArea) return false;
    mArea->dropGold(mClientId, count);
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
    const uint32_t bodyId,
    bool& bodyRemoved,
    eBodyItemsTaken& taken) {
    if(!mArea) return false;
    mArea->pickupBody(mClientId, bodyId,
                      bodyRemoved, taken);
    return true;
}

bool eServerClientHandler::changeTeam(
    const eTeamId newTeam) {
    if(!mArea) return false;
    return mArea->changeTeam(mClientId, newTeam);
}

bool eServerClientHandler::spawnPortal(
    uint32_t& portalId,
    uint8_t& mapId,
    uint8_t& areaId,
    ePointF& pos) {
    if(!mArea) return false;
    return mArea->spawnPortal(
        mClientId, portalId,
        mapId, areaId, pos);
}

bool eServerClientHandler::equipmentAction(
    const eEquipmentAction& a) {
    if(!mArea) return false;
    return mArea->equipmentAction(mClientId, a);
}

bool eServerClientHandler::buyAction(
    const eBuyAction& a, uint32_t& newItemId) {
    if(!mArea) return false;
    return mArea->buyAction(mClientId, a, newItemId);
}

bool eServerClientHandler::sellAction(
    const eSellAction& a) {
    if(!mArea) return false;
    return mArea->sellAction(mClientId, a);
}

bool eServerClientHandler::requestSeller(
    const uint32_t sellerId, eSeller& seller) {
    if(!mArea) return false;
    return mArea->requestSeller(
        mClientId, sellerId, seller);
}
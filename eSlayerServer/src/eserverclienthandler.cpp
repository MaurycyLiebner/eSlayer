#include "eserverclienthandler.h"

#include "eclientaction.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/eattackdata.h>

eServerClientHandler::eServerClientHandler(const int clientId) :
    mClientId(clientId) {}

bool eServerClientHandler::requestWeaponData() {
    return true;
}

bool eServerClientHandler::requestEquipment() {
    return true;
}

bool eServerClientHandler::receiveData(eRequestData& data,
                                       float& resultTime) {
    if(!mArea) return false;
    resultTime = mArea->time();
    mArea->unitsData(mClientId, data.fNewUnits, data.fUpdatedUnits);
    data.fMissiles = mArea->missileData(mClientId);
    mArea->itemsData(mClientId, data.fNewItems, data.fRemovedItemIds);
    return true;
}

bool eServerClientHandler::receiveWeaponData(eWeaponData& data) {
    if(!mArea) return false;
    const auto unit = mArea->unit(mClientId);
    if(!unit) return false;
    data = unit->weaponData();
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
    unit->fPos = u.fPos;
    unit->fVel = u.fVel;
    unit->fAngle = u.fAngle;

    if(unit->fActionTime <= 0.f) {
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
    const auto client = mArea->unit(mClientId);
    if(!client) return false;

    const auto a = client->action();
    a->setChild(nullptr);
    client->fHealth = client->fMaxHealth;
    client->fPos = ePointF{0.f, 0.f};
    return true;
}

bool eServerClientHandler::spawn(const eEquipment& eq,
                                 const eScreenDimensions& screenDims) {
    if(!mArea) return false;
    const auto client = mArea->unit(mClientId);
    if(client) return false;
    mArea->addClient(mClientId, eq, ePointF{0.f, 0.f},
                     screenDims);
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

bool eServerClientHandler::pickupItem(
    const int itemId, const bool drag) {
    if(!mArea) return false;
    return mArea->pickupItem(mClientId, itemId, drag);
}

bool eServerClientHandler::dropItem(
    const int itemId) {
    if(!mArea) return false;
    mArea->dropItem(mClientId, itemId);
    return true;
}

bool eServerClientHandler::rearrangeItems(
    const eEquipment& eq) {
    if(!mArea) return false;
    mArea->rearrangeItems(mClientId, eq);
    return true;
}

#include "eserverclienthandler.h"

#include "eclientaction.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/eattackdata.h>

eServerClientHandler::eServerClientHandler(const int clientId) :
    mClientId(clientId) {}

bool eServerClientHandler::requestData() {
    return true;
}

bool eServerClientHandler::receiveData(eRequestData& data,
                                       float& resultTime) {
    if(!mArea) return false;
    resultTime = mArea->time();
    const auto unitsData = mArea->unitsData(mClientId);
    data.fUnits = unitsData;
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
    case eAttackTargetType::character: {
        const auto u = mArea->unit(target.fChar);
        if(!u) return false;
        const auto a = client->action();
        if(const auto ca = dynamic_cast<eClientAction*>(a.get())) {
            ca->attack(u);
        }
    } break;
    case eAttackTargetType::position: {
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
        ca->attack(nullptr);
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

bool eServerClientHandler::spawn() {
    if(!mArea) return false;
    const auto client = mArea->unit(mClientId);
    if(client) return false;
    mArea->addClient(mClientId, ePointF{0.f, 0.f});
    return true;
}

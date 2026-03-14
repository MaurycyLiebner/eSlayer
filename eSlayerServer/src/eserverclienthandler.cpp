#include "eserverclienthandler.h"

#include "eclientaction.h"

#include <eSlayerHelpers/echardata.h>

eServerClientHandler::eServerClientHandler(const int clientId) :
    mClientId(clientId) {}

bool eServerClientHandler::requestUnits() {
    if(mArea) {
        const double time = mArea->time();
        const auto unitsData = mArea->unitsData(mClientId);
        mUnitRequests.emplace_back(eUnitsRequest{time, unitsData});
    } else {
        mUnitRequests.emplace_back(eUnitsRequest{0., {}});
    }
    return true;
}

bool eServerClientHandler::receiveUnits(std::vector<eUnitData>& units,
                                        double& resultTime,
                                        const double clientTime) {
    const int iMax = mUnitRequests.size();
    for(int i = 0; i < iMax; i++) {
        auto& r = mUnitRequests[i];
        resultTime = r.fTime;
        if(clientTime >= resultTime) {
            std::swap(units, r.fUnits);
            mUnitRequests.erase(mUnitRequests.begin() + i);
            return true;
        }
    }
    return false;
}

bool eServerClientHandler::moveTo(const ePointF& pos) {
    if(!mArea) return false;
    const auto unit = mArea->unit(mClientId);
    if(unit) {
        unit->fVel = ePointF::vector(pos, unit->fPos);
        unit->fAngle = unit->fVel.angle();
        unit->fPos = pos;
    } else {
        mArea->addClient(mClientId, pos);
    }
    return true;
}

bool eServerClientHandler::attack(const int targetId) {
    if(!mArea) return false;
    const auto client = mArea->unit(mClientId);
    if(!client) return false;
    const auto target = mArea->unit(targetId);
    if(!target) return false;

    const auto a = client->action();
    if(const auto ca = dynamic_cast<eClientAction*>(a.get())) {
        ca->attack(target);
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
    client->fPos = ePointF{0., 0.};

    return true;
}

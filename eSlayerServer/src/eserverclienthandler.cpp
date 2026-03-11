#include "eserverclienthandler.h"

bool eServerClientHandler::requestUnits() {
    if(mArea) {
        const double time = mArea->time();
        const auto& units = mArea->units();
        std::vector<eUnitData> unitsData;
        unitsData.reserve(units.size());
        for(const auto& u : units) {
            const eUnitData& uu =
                reinterpret_cast<eUnitData&>(*u);
            unitsData.emplace_back(uu);
        }
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

bool eServerClientHandler::moveTo(const int clientId, const ePointF& pos) {
    if(!mArea) return false;
    const auto unit = mArea->unit(clientId);
    if(unit) {
        unit->fVel = ePointF::vector(pos, unit->fPos);
        unit->fAngle = unit->fVel.angle();
        unit->fPos = pos;
    } else {
        mArea->addClient(clientId, pos);
    }
    return true;
}

bool eServerClientHandler::attack(const int clientId, const int targetId) {
    if(!mArea) return false;
    const auto unit = mArea->unit(targetId);
    unit->fHealth = std::max(0, unit->fHealth - 10);
    return true;
}

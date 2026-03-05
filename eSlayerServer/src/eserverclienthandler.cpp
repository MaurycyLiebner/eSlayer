#include "eserverclienthandler.h"

bool eServerClientHandler::requestUnits() {
    const auto now = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
        );
    if(mArea) {
        const auto& units = mArea->units();
        mUnitRequests.emplace_back(eUnitsRequest{now, units});
    } else {
        mUnitRequests.emplace_back(eUnitsRequest{now, {}});
    }
    return true;
}

int eServerClientHandler::receiveUnits(std::vector<std::shared_ptr<eServerUnit>>& units) {
    const auto now = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
        );
    const int iMax = mUnitRequests.size();
    for(int i = 0; i < iMax; i++) {
        auto& r = mUnitRequests[i];
        const auto& requestTime = r.fRequestTime;
        const auto d = std::chrono::duration_cast<milliseconds>(now - requestTime);
        const int di = d.count();
        if(di >= mDelay) {
            std::swap(units, r.fUnits);
            mUnitRequests.erase(mUnitRequests.begin() + i);
            return di;
        }
    }
    return -1;
}

bool eServerClientHandler::moveTo(const int clientId, const ePointF& pos) {
    if(!mArea) return false;
    const auto unit = mArea->unit(clientId);
    if(unit) {
        unit->fVel = ePointF::vector(pos, unit->fPos);
        unit->fPos = pos;
    } else {
        mArea->addClient(clientId, pos);
    }
    return true;
}

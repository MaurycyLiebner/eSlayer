#include "eSlayerHelpers/erequestdata.h"

void eRequestData::read(ePacket& p) {
    p >> fRequestId;

    uint16_t nUnits;
    p >> nUnits;
    for(int i = 0; i < nUnits; i++) {
        auto& u = fUnits.emplace_back();
        u.read(p);
    }

    uint16_t nMissiles;
    p >> nMissiles;
    for(int i = 0; i < nMissiles; i++) {
        auto& u = fMissiles.emplace_back();
        u.read(p);
    }
}

void eRequestData::write(ePacket& p) const {
    p << fRequestId;

    const uint16_t nUnits = fUnits.size();
    p << nUnits;
    for(const auto& u : fUnits) {
        u.write(p);
    }

    const uint16_t nMissiles = fMissiles.size();
    p << nMissiles;
    for(const auto& m : fMissiles) {
        m.write(p);
    }
}

#include "eSlayerHelpers/erequestdata.h"

void eRequestData::read(ePacket& p) {
    p >> fRequestId;
    int32_t nUnits;
    p >> nUnits;
    for(int i = 0; i < nUnits; i++) {
        auto& u = fUnits.emplace_back();
        u.read(p);
    }
}

void eRequestData::write(ePacket& p) const {
    p << fRequestId;
    const int32_t nUnits = fUnits.size();
    p << nUnits;
    for(const auto& u : fUnits) {
        u.write(p);
    }
}

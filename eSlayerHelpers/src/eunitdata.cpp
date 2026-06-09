#include "eSlayerHelpers/eunitdata.h"

#include "eSlayerHelpers/epacket.h"

const float radiusMax = 2.f;
const float angleMax = 360.f;
const float animSpeedMax = 100.f;
const float blockingActionTimeMax = 25.5f;

float eUnitData::sColdSpeed = 0.5f;

void eUnitData::read(ePacket& p) {
    eUnitDynamicData::read(p);
    p >> fTeamId;

    p >> fUnitInfoId;

    fRadius = p.readFloatU8(radiusMax);

    uint8_t nMods;
    p >> nMods;
    for(uint8_t i = 0; i < nMods; i++) {
        uint8_t m;
        p >> m;
        fMods.emplace(m);
    }

    fModelParts.read(p);
}

void eUnitData::write(ePacket& p) const {
    eUnitDynamicData::write(p);
    p << fTeamId;

    p << fUnitInfoId;

    p.writeFloatU8(fRadius, radiusMax);

    const uint8_t nMods = fMods.size();
    p << nMods;
    for(const auto m : fMods) {
        p << m;
    }

    fModelParts.write(p);
}

eUnitData eUnitData::toUnitData() const {
    eUnitData result = *this;
    result.fUpdate = std::numeric_limits<decltype(result.fUpdate)>::max();
    return result;
}

eUnitDynamicData eUnitData::toDynamicData(const uint8_t update) const {
    eUnitDynamicData result = *this;
    result.fUpdate = update;
    return result;
}

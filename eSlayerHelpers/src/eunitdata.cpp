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

    p.read8(fMods);

    fModelParts.read(p);
}

void eUnitData::write(ePacket& p) const {
    eUnitDynamicData::write(p);
    p << fTeamId;

    p << fUnitInfoId;

    p.writeFloatU8(fRadius, radiusMax);

    p.write8(fMods);

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

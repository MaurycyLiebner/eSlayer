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

    p >> fCharDataId;

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

    p << fCharDataId;

    p.writeFloatU8(fRadius, radiusMax);

    const uint8_t nMods = fMods.size();
    p << nMods;
    for(const auto m : fMods) {
        p << m;
    }

    fModelParts.write(p);
}

bool eUnitData::cold() const {
    return fState & 1u;
}

void eUnitData::setCold(const bool c) {
    if(c) {
        fState |= 1u;
    } else {
        fState &= ~1u;
    }
}

bool eUnitData::frozen() const {
    return fState & 2u;
}

void eUnitData::setFrozen(const bool f) {
    if(f) {
        fState |= 2u;
    } else {
        fState &= ~2u;
    }
}

bool eUnitData::poisoned() const {
    return fState & 4u;
}

void eUnitData::setPoisoned(const bool p) {
    if(p) {
        fState |= 4u;
    } else {
        fState &= ~4u;
    }
}

void eUnitData::removeBoostData(const uint8_t id) {
    fBoosts.erase(id);
}

void eUnitData::addBoostData(const uint8_t id) {
    fBoosts.emplace(id);
}

eUnitData eUnitData::toUnitData() const {
    return *this;
}

eUnitDynamicData eUnitData::toDynamicData() const {
    return static_cast<const eUnitDynamicData&>(*this);
}

eDeadUnitDynamicData eUnitData::toDynamicDeadData() const {
    const auto& u = static_cast<const eUnitDynamicDataBase&>(*this);
    return reinterpret_cast<const eDeadUnitDynamicData&>(u);
}

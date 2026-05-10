#include "eSlayerHelpers/eunitdata.h"

#include "eSlayerHelpers/epacket.h"

const float radiusMax = 2.f;
const float angleMax = 360.f;
const float animSpeedMax = 100.f;
const float blockingActionTimeMax = 25.5f;

void eUnitData::read(ePacket& p) {
    p >> fCharId;
    p >> fTeamId;

    p >> fCharDataId;

    fRadius = p.readFloatU8(radiusMax);

    p >> fPos;
    fAngle = p.readFloatU8(angleMax);

    p >> fAnim;
    p >> fAnimId;
    fAnimSpeed = p.readFloatU16(animSpeedMax);

    fBlockingActionTime = p.readFloatU8(blockingActionTimeMax);

    p >> fHealth;
    p >> fMaxHealth;

    uint8_t nMods;
    p >> nMods;
    for(uint8_t i = 0; i < nMods; i++) {
        p >> fMods.emplace_back();
    }

    fModelParts.read(p);
}

void eUnitData::write(ePacket& p) const {
    p << fCharId;
    p << fTeamId;

    p << fCharDataId;

    p.writeFloatU8(fRadius, radiusMax);

    p << fPos;
    p.writeFloatU8(fAngle, angleMax);

    p << fAnim;
    p << fAnimId;
    p.writeFloatU16(fAnimSpeed, animSpeedMax);

    p.writeFloatU8(fBlockingActionTime, blockingActionTimeMax);

    p << fHealth;
    p << fMaxHealth;

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

bool eUnitData::poisoned() const {
    return fState & 2u;
}

void eUnitData::setPoisoned(const bool p) {
    if(p) {
        fState |= 2u;
    } else {
        fState &= ~2u;
    }
}

eUnitData eUnitData::toUnitData() const {
    eUnitData d;
    d.fPos = fPos;
    d.fCharId = fCharId;
    d.fTeamId = fTeamId;
    d.fCharDataId = fCharDataId;
    d.fRadius = fRadius;
    d.fAngle = fAngle;
    d.fAnim = fAnim;
    d.fAnimId = fAnimId;
    d.fAnimSpeed = fAnimSpeed;
    d.fBlockingActionTime = fBlockingActionTime;
    d.fHealth = fHealth;
    d.fMaxHealth = fMaxHealth;
    d.fMods = fMods;
    d.fModelParts = fModelParts;
    d.fState = fState;
    return d;
}

eUnitDynamicData eUnitData::toDynamicData() const {
    eUnitDynamicData d;
    d.fCharId = fCharId;
    d.fPos = fPos;
    d.fAngle = fAngle;
    d.fAnim = fAnim;
    d.fAnimId = fAnimId;
    d.fAnimSpeed = fAnimSpeed;
    d.fBlockingActionTime = fBlockingActionTime;
    d.fHealth = fHealth;
    d.fMaxHealth = fMaxHealth;
    d.fState = fState;
    return d;
}

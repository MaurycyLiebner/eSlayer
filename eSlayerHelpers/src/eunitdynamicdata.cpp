#include "eSlayerHelpers/eunitdynamicdata.h"

#include "eSlayerHelpers/epacket.h"

const float angleMax = 360.f;
const float animSpeedMax = 100.f;
const float blockingActionTimeMax = 25.5f;

void eUnitDynamicData::read(ePacket& p) {
    p >> fCharId;

    p >> fPos;
    fAngle = p.readFloatU8(angleMax);

    p >> fAnim;
    p >> fAnimId;
    fAnimSpeed = p.readFloatU16(animSpeedMax);

    fBlockingActionTime = p.readFloatU8(blockingActionTimeMax);

    p >> fHealth;
    p >> fMaxHealth;

    p >> fState;

    uint8_t nBoosts;
    p >> nBoosts;
    for(int i = 0; i < nBoosts; i++) {
        uint8_t b;
        p >> b;
        fBoosts.emplace(b);
    }
}

void eUnitDynamicData::write(ePacket& p) const {
    p << fCharId;

    p << fPos;
    p.writeFloatU8(fAngle, angleMax);

    p << fAnim;
    p << fAnimId;
    p.writeFloatU16(fAnimSpeed, animSpeedMax);

    p.writeFloatU8(fBlockingActionTime, blockingActionTimeMax);

    p << fHealth;
    p << fMaxHealth;

    p << fState;

    const uint8_t nBoosts = fBoosts.size();
    p << nBoosts;
    for(const uint8_t b : fBoosts) {
        p << b;
    }
}

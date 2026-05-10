#include "eSlayerHelpers/eunitdynamicdata.h"

#include "eSlayerHelpers/epacket.h"

void eUnitDynamicData::read(ePacket& p) {
    p >> fCharId;

    p >> fPos;
    p >> fAngle;

    p >> fAnim;
    p >> fAnimId;
    p >> fAnimSpeed;

    p >> fBlockingActionTime;

    p >> fHealth;
    p >> fMaxHealth;

    p >> fState;
}

void eUnitDynamicData::write(ePacket& p) const {
    p << fCharId;

    p << fPos;
    p << fAngle;

    p << fAnim;
    p << fAnimId;
    p << fAnimSpeed;

    p << fBlockingActionTime;

    p << fHealth;
    p << fMaxHealth;

    p << fState;
}

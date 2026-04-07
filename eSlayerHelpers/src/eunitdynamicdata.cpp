#include "eSlayerHelpers/eunitdynamicdata.h"

#include "eSlayerHelpers/epacket.h"

void eUnitDynamicData::read(ePacket& p) {
    p >> fCharId;

    p >> fPos;
    p >> fVel;
    p >> fAngle;

    p >> fAnim;
    p >> fAnimId;
    p >> fAnimSpeed;

    p >> fBlockingActionTime;

    p >> fHealth;
    p >> fMaxHealth;
}

void eUnitDynamicData::write(ePacket& p) const {
    p << fCharId;

    p << fPos;
    p << fVel;
    p << fAngle;

    p << fAnim;
    p << fAnimId;
    p << fAnimSpeed;

    p << fBlockingActionTime;

    p << fHealth;
    p << fMaxHealth;
}

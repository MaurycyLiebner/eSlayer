#include "eSlayerHelpers/eunitdata.h"

void eUnitData::read(ePacket& p) {
    p >> fCharId;
    p >> fTeamId;

    p >> fTypeId;

    p >> fRadius;

    p >> fPos;
    p >> fVel;
    p >> fAngle;

    p >> fAnim;
    p >> fAnimId;
    p >> fAnimSpeed;

    p >> fActionTime;

    p >> fHealth;
    p >> fMaxHealth;

    fModelParts.read(p);
}

void eUnitData::write(ePacket& p) const {
    p << fCharId;
    p << fTeamId;

    p << fTypeId;

    p << fRadius;

    p << fPos;
    p << fVel;
    p << fAngle;

    p << fAnim;
    p << fAnimId;
    p << fAnimSpeed;

    p << fActionTime;

    p << fHealth;
    p << fMaxHealth;

    fModelParts.write(p);
}

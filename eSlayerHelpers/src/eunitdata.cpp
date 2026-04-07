#include "eSlayerHelpers/eunitdata.h"

void eUnitData::read(ePacket& p) {
    p >> fCharId;
    p >> fTeamId;

    p >> fCharDataId;

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

    p << fCharDataId;

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

eUnitData eUnitData::toUnitData() const {
    eUnitData d;
    d.fPos = fPos;
    d.fCharId = fCharId;
    d.fTeamId = fTeamId;
    d.fCharDataId = fCharDataId;
    d.fRadius = fRadius;
    d.fVel = fVel;
    d.fAngle = fAngle;
    d.fAnim = fAnim;
    d.fAnimId = fAnimId;
    d.fAnimSpeed = fAnimSpeed;
    d.fActionTime = fActionTime;
    d.fHealth = fHealth;
    d.fMaxHealth = fMaxHealth;
    d.fModelParts = fModelParts;
    return d;
}

eUnitDynamicData eUnitData::toDynamicData() const {
    eUnitDynamicData d;
    d.fCharId = fCharId;
    d.fPos = fPos;
    d.fVel = fVel;
    d.fAngle = fAngle;
    d.fAnim = fAnim;
    d.fAnimId = fAnimId;
    d.fAnimSpeed = fAnimSpeed;
    d.fActionTime = fActionTime;
    d.fHealth = fHealth;
    d.fMaxHealth = fMaxHealth;
    return d;
}

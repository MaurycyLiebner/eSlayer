#include "eSlayerHelpers/emissile.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/eskills.h"

void eMissile::read(ePacket& p) {
    p >> fId;
    p >> fTeamId;
    p >> fType;
    p >> fPathType;
    p >> fFrom;
    p >> fPos;
    p >> fTo;
    fSpeed = p.readFloatU8(eSkill::sSpeedMax);
    p >> fToPierce;
    fRemDist = p.readFloatU8(eSkill::sRangeMax);
    fRemTime = p.readFloatU8(eSkill::sTimeMax);
    fRadius = p.readFloatU8(eSkill::sRadiusMax);
}

void eMissile::write(ePacket& p) const {
    p << fId;
    p << fTeamId;
    p << fType;
    p << fPathType;
    p << fFrom;
    p << fPos;
    p << fTo;
    p.writeFloatU8(fSpeed, eSkill::sSpeedMax);
    p << fToPierce;
    p.writeFloatU8(fRemDist, eSkill::sRangeMax);
    p.writeFloatU8(fRemTime, eSkill::sTimeMax);
    p.writeFloatU8(fRadius, eSkill::sRadiusMax);
}

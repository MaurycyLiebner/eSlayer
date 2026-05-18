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
    fRemDistTime = p.readFloatU16(eSkill::sRangeTimeMax);
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
    p.writeFloatU16(fRemDistTime, eSkill::sRangeTimeMax);
    p.writeFloatU8(fRadius, eSkill::sRadiusMax);
}

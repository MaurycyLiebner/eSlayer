#include "eSlayerHelpers/emissile.h"

#include "eSlayerHelpers/epacket.h"

const float speedMax = 2.55f;
const float remDistTimeMax = 655.35f;
const float radiusMax = 2.55f;

void eMissile::read(ePacket& p) {
    p >> fId;
    p >> fTeamId;
    p >> fType;
    p >> fPathType;
    p >> fFrom;
    p >> fPos;
    p >> fTo;
    fSpeed = p.readFloatU8(speedMax);
    p >> fToPierce;
    fRemDistTime = p.readFloatU16(remDistTimeMax);
    fRadius = p.readFloatU8(radiusMax);
}

void eMissile::write(ePacket& p) const {
    p << fId;
    p << fTeamId;
    p << fType;
    p << fPathType;
    p << fFrom;
    p << fPos;
    p << fTo;
    p.writeFloatU8(fSpeed, speedMax);
    p << fToPierce;
    p.writeFloatU16(fRemDistTime, remDistTimeMax);
    p.writeFloatU8(fRadius, radiusMax);
}

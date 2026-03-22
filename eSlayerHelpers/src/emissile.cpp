#include "eSlayerHelpers/emissile.h"

#include "eSlayerHelpers/epacket.h"

void eMissile::read(ePacket& p) {
    p >> fId;
    p >> fTeamId;
    p >> fType;
    p >> fPathType;
    p >> fFrom;
    p >> fPos;
    p >> fTo;
    p >> fSpeed;
    p >> fPierced;
    p >> fRemDist;
    p >> fRadius;
    p >> fTime;
}

void eMissile::write(ePacket& p) const {
    p << fId;
    p << fTeamId;
    p << fType;
    p << fPathType;
    p << fFrom;
    p << fPos;
    p << fTo;
    p << fSpeed;
    p << fPierced;
    p << fRemDist;
    p << fRadius;
    p << fTime;
}

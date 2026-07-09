#include "eSlayerHelpers/emercenary.h"

#include "eSlayerHelpers/epacket.h"

void eMercenary::read(ePacket& p) {
    p >> fUnitId;
    p >> fMercType;
    p >> fNameId;
    p >> fLevel;
    p >> fExp;
    fEq.read(p);
    p >> fDead;
}

void eMercenary::write(ePacket& p) const {
    p << fUnitId;
    p << fMercType;
    p << fNameId;
    p << fLevel;
    p << fExp;
    fEq.write(p);
    p << fDead;
}

#include "eSlayerHelpers/edamage.h"

#include "eSlayerHelpers/epacket.h"

void eDamage::read(ePacket& p) {
    p >> fPhysical;
    p >> fFire;
    p >> fCold;
    p >> fLightning;
}

void eDamage::write(ePacket& p) const {
    p << fPhysical;
    p << fFire;
    p << fCold;
    p << fLightning;
}

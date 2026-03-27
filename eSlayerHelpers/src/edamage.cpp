#include "eSlayerHelpers/edamage.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/erand.h"

eDamage eDamage::sRandom(const eDamage& min,
                         const eDamage& max) {
    eDamage result;
    result.fPhysical = eRand::randF(min.fPhysical, max.fPhysical);
    result.fFire = eRand::randF(min.fFire, max.fFire);
    result.fCold = eRand::randF(min.fCold, max.fCold);
    result.fLightning = eRand::randF(min.fLightning, max.fLightning);
    return result;
}

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

#include "eSlayerHelpers/edamage.h"

#include "eSlayerHelpers/erand.h"

eDamage eDamage::sRandom(const eDamage& min,
                         const eDamage& max) {
    eDamage result;
    result.fPhysical = eRand::randF(min.fPhysical, max.fPhysical);
    result.fFire = eRand::randF(min.fFire, max.fFire);
    result.fCold = eRand::randF(min.fCold, max.fCold);
    result.fLightning = eRand::randF(min.fLightning, max.fLightning);
    result.fPoison = eRand::randF(min.fPoison, max.fPoison);
    result.fPoisonFrameLength = eRand::randF(min.fPoisonFrameLength, max.fPoisonFrameLength);
    return result;
}

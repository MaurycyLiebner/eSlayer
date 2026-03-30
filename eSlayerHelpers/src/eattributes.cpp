#include "eSlayerHelpers/eattributes.h"

#include "eSlayerHelpers/epacket.h"

#include <cmath>

float eAttributes::nextLevelExp() const {
    return 500.f*std::pow(fLevel, 1.5f);
}

void eAttributes::read(ePacket& p) {
    p >> fLevel;
    p >> fStrength;
    p >> fDexterity;
    p >> fVitality;
    p >> fEnergy;
}

void eAttributes::write(ePacket& p) const {
    p << fLevel;
    p << fStrength;
    p << fDexterity;
    p << fVitality;
    p << fEnergy;
}

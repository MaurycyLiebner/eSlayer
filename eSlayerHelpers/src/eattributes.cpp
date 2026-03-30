#include "eSlayerHelpers/eattributes.h"

#include "eSlayerHelpers/epacket.h"

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

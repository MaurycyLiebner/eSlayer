#include "eSlayerHelpers/emodifier.h"

#include "eSlayerHelpers/epacket.h"

void eModifier::read(ePacket& p) {
    p >> fType;
    p >> fValue1;
    p >> fValue2;
}

void eModifier::write(ePacket& p) const {
    p << fType;
    p << fValue1;
    p << fValue2;
}

#include "eSlayerHelpers/eitemmodifier.h"

#include "eSlayerHelpers/epacket.h"

void eItemModifier::read(ePacket& p) {
    p >> fType;
    p >> fValue1;
    p >> fValue2;
}

void eItemModifier::write(ePacket& p) const {
    p << fType;
    p << fValue1;
    p << fValue2;
}

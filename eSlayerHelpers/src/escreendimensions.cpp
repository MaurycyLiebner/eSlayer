#include "eSlayerHelpers/escreendimensions.h"

#include "eSlayerHelpers/epacket.h"

void eScreenDimensions::read(ePacket& p) {
    p >> fWidth;
    p >> fHeight;
}

void eScreenDimensions::write(ePacket& p) const {
    p << fWidth;
    p << fHeight;
}

#include "eSlayerHelpers/egrounditem.h"

#include "eSlayerHelpers/epacket.h"

void eGroundItem::read(ePacket& p) {
    p >> fItemId;
    p >> fDataId;
    p >> fType;
    p >> fSubType;
    p >> fPos;
}

void eGroundItem::write(ePacket& p) const {
    p << fItemId;
    p << fDataId;
    p << fType;
    p << fSubType;
    p << fPos;
}

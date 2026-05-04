#include "eSlayerHelpers/eitem.h"

#include "eSlayerHelpers/epacket.h"

void eItem::read(ePacket& p) {
    p >> fItemId;

    p >> fDataId;
    p >> fType;
    p >> fSubType;

    p >> fRarity;
    p >> fSockets;

    p >> fRequiredLevel;

    p >> fMinDmg;
    p >> fMaxDmg;
    p >> fDefense;
    p >> fBlockChance;

    uint8_t nmods;
    p >> nmods;
    for(int i = 0; i < nmods; i++) {
        auto& mod = fModifiers.emplace_back();
        mod.read(p);
    }
}

void eItem::write(ePacket& p) const {
    p << fItemId;

    p << fDataId;
    p << fType;
    p << fSubType;

    p << fRarity;
    p << fSockets;

    p << fRequiredLevel;

    p << fMinDmg;
    p << fMaxDmg;
    p << fDefense;
    p << fBlockChance;

    const uint8_t nmods = fModifiers.size();
    p << nmods;
    for(const auto& mod : fModifiers) {
        mod.write(p);
    }
}

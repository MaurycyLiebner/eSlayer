#include "eSlayerHelpers/eitem.h"

#include "eSlayerHelpers/epacket.h"

uint32_t eItem::calculateCost() const {
    if(fType == eItemType::none) return 0;
    const auto& mods = fModifiers;
    const int nmods = mods.size();
    const int l = fRequiredLevel;
    return 20*((1 + l*l)*(2 + nmods));
}

uint32_t eItem::calculateSellCost() const {
    const uint32_t cost = calculateCost();
    return std::min(cost, 35000u);
}

void eItem::read(ePacket& p) {
    p >> fItemId;

    p >> fPrefix;
    p >> fSuffix;

    p >> fDataId;
    p >> fType;
    p >> fSubType;

    p >> fRarity;
    p >> fSockets;

    p >> fCount;

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

    p << fPrefix;
    p << fSuffix;

    p << fDataId;
    p << fType;
    p << fSubType;

    p << fRarity;
    p << fSockets;

    p << fCount;

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
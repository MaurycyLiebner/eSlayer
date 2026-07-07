#ifndef EITEM_H
#define EITEM_H

#include "eitemrarity.h"
#include "emodifier.h"

#include "eitemdata.h"

#include <vector>

struct ESLAYERHELPERS_API eItemBase {
    uint32_t fItemId = 0;

    uint8_t fPrefix = 0;
    uint8_t fSuffix = 0;

    uint8_t fDataId = 0;
    eItemType fType = eItemType::none;
    uint8_t fSubType = 0;

    eItemRarity fRarity = eItemRarity::normal;
    uint8_t fSockets = 0;

    uint32_t fCount = 0;
};

struct ESLAYERHELPERS_API eItem :
        public eItemBase {
    uint8_t fRequiredLevel = 0;

    uint16_t fMinDmg = 0; // min weapon / shield / boots damage
    uint16_t fMaxDmg = 0; // max weapon weapon / shield / boots damage
    uint16_t fDefense = 0; // armor defense
    uint16_t fBlockChance = 0; // shield block chance
    std::vector<eModifier> fModifiers;

    std::vector<eItem> fJewels;

    uint32_t calculateCost() const;
    uint32_t calculateSellCost() const;

    bool spaceForJewel() const;
    bool addJewel(const eItem& jewel);
    bool addSocket();

    void read(ePacket& p);
    void write(ePacket& p) const;
};

enum class eHoverItemType {
    regular, buy, sell
};

struct ESLAYERHELPERS_API eHoverItem {
    eHoverItemType fType = eHoverItemType::regular;
    eItem fItem;
};

#endif // EITEM_H

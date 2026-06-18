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

struct ESLAYERHELPERS_API eItem : public eItemBase {
    uint8_t fRequiredLevel = 0;

    uint16_t fMinDmg = 0; // min weapon / shield / boots damage
    uint16_t fMaxDmg = 0; // max weapon weapon / shield / boots damage
    uint16_t fDefense = 0; // armor defense
    uint16_t fBlockChance = 0; // shield block chance
    std::vector<eModifier> fModifiers;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EITEM_H

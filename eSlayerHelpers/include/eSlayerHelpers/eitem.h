#ifndef EITEM_H
#define EITEM_H

#include "eitemrarity.h"
#include "emodifier.h"

#include "eitemdata.h"

#include <vector>

struct ESLAYERHELPERS_API eItem {
    uint32_t fItemId = 0;

    uint8_t fDataId = 0;
    eItemType fType = eItemType::none;
    uint8_t fSubType = 0;

    eItemRarity fRarity = eItemRarity::normal;
    uint8_t fSockets = 0;

    uint8_t fRequiredLevel = 0;

    float fMinDmg = 0.f; // min weapon / shield / boots damage
    float fMaxDmg = 0.f; // max weapon weapon / shield / boots damage
    float fDefense = 0.f; // armor defense
    float fBlockChance = 0.f; // shield block chance
    std::vector<eModifier> fModifiers;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EITEM_H

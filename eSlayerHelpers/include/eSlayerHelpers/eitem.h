#ifndef EITEM_H
#define EITEM_H

#include "emodifier.h"

#include "eitemdata.h"

#include <vector>

struct ESLAYERHELPERS_API eItem {
    uint8_t fDataId = 0;
    eItemType fType = eItemType::none;
    uint8_t fSubType = 0;
    float fValue1 = 0.f; // min weapon / shield / boots damage
    float fValue2 = 0.f; // max weapon weapon / shield / boots damage
    float fValue3 = 0.f; // armor defense
    float fValue4 = 0.f; // shield block chance
    std::vector<eModifier> fModifiers;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EITEM_H

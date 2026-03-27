#ifndef EITEM_H
#define EITEM_H

#include "emodifier.h"

#include <vector>

enum class eItemType : uint8_t {
    none,
    boots,
    gloves,
    helmet,
    armor,
    belt,
    ring,
    amulet,
    weapon,
    shield,
    arrows
};

struct ESLAYERHELPERS_API eItem {
    eItemType fType;
    uint8_t fSubType;
    float fValue1; // min weapon / shield / boots damage
    float fValue2; // max weapon weapon / shield / boots damage
    float fValue3; // armor defense
    float fValue4; // shield block chance
    std::vector<eModifier> fModifiers;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EITEM_H

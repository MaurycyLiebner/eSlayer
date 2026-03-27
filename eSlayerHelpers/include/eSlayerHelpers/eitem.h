#ifndef EITEM_H
#define EITEM_H

#include "eitemmodifier.h"

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
    float fValue1; // armor defense / min weapon damage
    float fValue2; // shield block chance / max weapon damage
    float fValue3; // min boots / shields damage
    float fValue4; // max boots / shields damage
    std::vector<eItemModifier> fModifiers;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EITEM_H

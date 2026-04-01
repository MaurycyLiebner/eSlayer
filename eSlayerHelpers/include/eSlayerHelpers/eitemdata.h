#ifndef EITEMDATA_H
#define EITEMDATA_H

#include <cstdint>

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

enum class eItemRarity : uint8_t {
    normal,
    magic,
    rare,
    set,
    unique
};

struct eItemData {
    eItemType fType = eItemType::none;
    uint8_t fSubType = 0;

    uint8_t fWidth;
    uint8_t fHeight;

    uint8_t fLevelReq = 0;
    uint8_t fStrengthReq = 0;
    uint8_t fDexterityReq = 0;
};

#endif // EITEMDATA_H

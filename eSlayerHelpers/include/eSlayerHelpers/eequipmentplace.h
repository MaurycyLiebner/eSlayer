#ifndef EEQUIPMENTPLACE_H
#define EEQUIPMENTPLACE_H

#include <cstdint>

enum class ePlaceType {
    none,
    boots,
    gloves,
    helmet,
    armor,
    belt,
    ringL,
    ringR,
    amulet,
    weapon1L,
    weapon1R,
    weapon2L,
    weapon2R,
    dragged,
    inventory,
    stash,
    beltPotions,
    beltHiddenPotions
};

struct eEquipmentPlace {
    ePlaceType fType = ePlaceType::none;
    uint8_t fX;
    uint8_t fY;
};

#endif // EEQUIPMENTPLACE_H

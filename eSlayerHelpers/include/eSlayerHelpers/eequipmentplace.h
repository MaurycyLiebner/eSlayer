#ifndef EEQUIPMENTPLACE_H
#define EEQUIPMENTPLACE_H

#include "eslayerhelpersexport.h"

#include <cstdint>
#include <string>

enum class ePlaceType : uint8_t {
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
    beltHiddenPotions,
    temporary
};

struct eEquipmentPlace {
    ePlaceType fType = ePlaceType::none;
    uint8_t fX;
    uint8_t fY;
};

namespace ePlaceTypeHelpers {
    ESLAYERHELPERS_API
    const std::string& name(const ePlaceType type);
    ESLAYERHELPERS_API
    ePlaceType type(const std::string& name);
}

#endif // EEQUIPMENTPLACE_H

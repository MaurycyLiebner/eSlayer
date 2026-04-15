#ifndef EPOTIONTYPE_H
#define EPOTIONTYPE_H

#include "eslayerhelpersexport.h"

#include <stdint.h>
#include <string>

enum class ePotionType : uint8_t {
    none,

    minorHealing,
    lightHealing,
    healing,
    greaterHealing,

    minorMana,
    lightMana,
    mana,
    greaterMana,

    minorRejuvenation,
    lightRejuvenation,
    rejuvenation,
    greaterRejuvenation,
};

namespace ePotionTypeHelpers {
    ESLAYERHELPERS_API
    std::string name(const ePotionType type);
    ESLAYERHELPERS_API
    ePotionType type(const std::string& name);
}

#endif // EPOTIONTYPE_H

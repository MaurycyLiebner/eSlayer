#ifndef EWEAPONTYPE_H
#define EWEAPONTYPE_H

#include "eslayerhelpersexport.h"

#include <stdint.h>
#include <string>

enum class eWeaponType : uint8_t {
    none, meele, shield, ranged, throwable
};

namespace eWeaponTypeHelpers {
    ESLAYERHELPERS_API
    std::string name(const eWeaponType type);
    ESLAYERHELPERS_API
    eWeaponType type(const std::string& name);
}

#endif // EWEAPONTYPE_H

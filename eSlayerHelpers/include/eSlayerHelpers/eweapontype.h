#ifndef EWEAPONTYPE_H
#define EWEAPONTYPE_H

#include <stdint.h>

enum class eWeaponType : uint8_t {
    none, meele, shield, ranged, throwable
};

enum class eWeaponSubtype : uint8_t {
    sword, longSword, pike, bow, throwingAxe
};

#endif // EWEAPONTYPE_H

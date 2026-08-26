#ifndef EUNITHIT_H
#define EUNITHIT_H

#include <cstdint>

enum class eSourceType : uint8_t {
    meele,
    other
};

enum class eHitType : uint8_t {
    hit, miss, block
};

struct eUnitHit {
    uint32_t fUnitId;
    eHitType fType;
    uint32_t fSourceId;
    eSourceType fSource;
    uint8_t fWeaponType;
};

#endif // EUNITHIT_H

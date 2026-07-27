#ifndef EATTRIBUTES_H
#define EATTRIBUTES_H

#include "eslayerhelpersexport.h"

#include <cstdint>

struct ESLAYERHELPERS_API eAttributes {
    uint8_t fLevel = 1;
    float fExp = 0.f;
    uint16_t fStrength = 0;
    uint16_t fDexterity = 0;
    uint16_t fVitality = 0;
    uint16_t fEnergy = 0;

    uint16_t fStatPoints = 0;

    uint32_t nextLevelExp(const int uinfoId) const;
    bool levelUp(const int uinfoId);

    uint32_t totalPoints() const;

    static bool samePoints(
        const eAttributes& attrs1,
        const eAttributes& attrs2);
};

#endif // EATTRIBUTES_H

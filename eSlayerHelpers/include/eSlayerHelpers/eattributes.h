#ifndef EATTRIBUTES_H
#define EATTRIBUTES_H

#include "eslayerhelpersexport.h"

#include <cstdint>
#include <vector>

struct ESLAYERHELPERS_API eAttributes {
    uint8_t fLevel = 1;
    float fExp = 0.f;
    uint16_t fStrength = 20;
    uint16_t fDexterity = 20;
    uint16_t fVitality = 20;
    uint16_t fEnergy = 15;

    static uint16_t sStatPointsPerLevel;
    static uint16_t sSkillPointsPerLevel;
    uint16_t fStatPoints = 0;

    uint32_t nextLevelExp() const;
    bool levelUp();

    uint32_t totalPoints() const;

    static bool samePoints(
        const eAttributes& attrs1,
        const eAttributes& attrs2);

    static void load();
private:
    static uint8_t sMaxLevel;
    static std::vector<uint32_t> sLevelExperience;
};

#endif // EATTRIBUTES_H

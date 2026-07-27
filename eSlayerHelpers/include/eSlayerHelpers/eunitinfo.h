#ifndef EUNITINFO_H
#define EUNITINFO_H

#include "ecolor.h"
#include "emodifier.h"

struct eUnitInfo {
    int fCharData;
    float fRadius;
    float fWalkSpeed;
    float fRunSpeed;
    float fLighting;
    int fLevel;
    eColor fColor;

    int fMissile;
    float fMissileRange;

    float fFleeChance;
    float fFleeDistance;

    float fTanChance;
    float fTanDistance;

    float fStandChance;
    int fStandLength;

    bool fDifficultyPenalties;

    std::map<uint16_t, uint16_t> fSkills;
    std::vector<eModifier> fModifiers;
    std::vector<uint8_t> fItems;

    std::vector<uint32_t> fLevelExperience;
    uint8_t fMaxLevel = 1;
    uint8_t fSkillPointsPerLevel = 1;
    uint8_t fStatPointsPerLevel = 5;
};

#endif // EUNITINFO_H

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

    float fFleeChance;
    float fFleeDistance;

    float fTanChance;
    float fTanDistance;

    bool fDifficultyPenalties;

    std::map<uint16_t, uint16_t> fSkills;
    std::vector<eModifier> fModifiers;
    std::vector<uint8_t> fItems;
};

#endif // EUNITINFO_H

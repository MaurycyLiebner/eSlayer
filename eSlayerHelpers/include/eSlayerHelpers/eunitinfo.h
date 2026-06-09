#ifndef EUNITINFO_H
#define EUNITINFO_H

#include "emodifier.h"
#include "ecolor.h"

struct eUnitInfo {
    int fCharData;
    float fRadius;
    float fWalkSpeed;
    float fRunSpeed;
    float fLighting;
    int fLevel;
    eColor fColor;

    std::map<uint16_t, uint16_t> fSkills;
    std::vector<eModifier> fModifiers;
    std::vector<uint8_t> fItems;
};

#endif // EUNITINFO_H

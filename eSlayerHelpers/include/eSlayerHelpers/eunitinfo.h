#ifndef EUNITINFO_H
#define EUNITINFO_H

#include "emodifier.h"
#include "ecolor.h"

struct eUnitInfo {
    int fCharData;
    float fRadius;
    float fWalkSpeed;
    float fRunSpeed;
    int fLevel;
    eColor fColor;

    std::map<uint16_t, uint16_t> fSkills;
    std::vector<eModifier> fModifiers;
};

#endif // EUNITINFO_H

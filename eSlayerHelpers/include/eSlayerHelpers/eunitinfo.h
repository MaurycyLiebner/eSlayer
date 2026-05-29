#ifndef EUNITINFO_H
#define EUNITINFO_H

#include "emodifier.h"

struct eUnitInfo {
    int fCharData;
    float fRadius;
    float fWalkSpeed;
    float fRunSpeed;
    int fLevel;

    std::map<uint16_t, uint16_t> fSkills;
    std::vector<eModifier> fModifiers;
};

#endif // EUNITINFO_H

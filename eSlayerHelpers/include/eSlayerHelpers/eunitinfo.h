#ifndef EUNITINFO_H
#define EUNITINFO_H

#include "emodifier.h"

struct eUnitInfo {
    int fCharData;
    float fRadius;
    float fWalkSpeed;
    float fRunSpeed;

    std::vector<eModifier> fModifiers;
};

#endif // EUNITINFO_H

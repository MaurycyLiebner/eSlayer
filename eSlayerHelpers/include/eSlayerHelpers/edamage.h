#ifndef EDAMAGE_H
#define EDAMAGE_H

#include "eslayerhelpersexport.h"

#include "eSlayerHelpers/erunsettings.h"

struct ESLAYERHELPERS_API eDamage {
    float fPhysical = 0.f;
    float fFire = 0.f;
    float fCold = 0.f;
    float fLightning = 0.f;
    float fPoisonPerFrame = 0.f;
    float fPoisonFrameLength = 0.f;

    float total() const {
        return fPhysical +
               fFire +
               fCold +
               fLightning +
               fPoisonPerFrame/eRunSettings::sFPS;
    }

    eDamage operator/(const float div) const {
        eDamage result;
        result.fPhysical = fPhysical/div;
        result.fFire = fFire/div;
        result.fCold = fCold/div;
        result.fLightning = fLightning/div;
        result.fPoisonPerFrame = fPoisonPerFrame/div;
        return result;
    }

    eDamage operator*(const eDamage& mult) const {
        eDamage result;
        result.fPhysical = fPhysical*mult.fPhysical;
        result.fFire = fFire*mult.fFire;
        result.fCold = fCold*mult.fCold;
        result.fLightning = fLightning*mult.fLightning;
        result.fPoisonPerFrame = fPoisonPerFrame*mult.fPoisonPerFrame;
        return result;
    }

    static eDamage sRandom(const eDamage& min,
                           const eDamage& max);
};

#endif // EDAMAGE_H

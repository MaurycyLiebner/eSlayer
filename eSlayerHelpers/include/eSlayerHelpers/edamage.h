#ifndef EDAMAGE_H
#define EDAMAGE_H

#include "eslayerhelpersexport.h"

#include <algorithm>

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
               fPoisonPerFrame/25.f;
    }

    eDamage operator/(const float div) const {
        eDamage result;
        result.fPhysical = fPhysical/div;
        result.fFire = fFire/div;
        result.fCold = fCold/div;
        result.fLightning = fLightning/div;
        result.fPoisonPerFrame = fPoisonPerFrame/div;
        result.fPoisonFrameLength = fPoisonFrameLength;
        return result;
    }

    eDamage operator*(const float mult) const {
        eDamage result;
        result.fPhysical = fPhysical*mult;
        result.fFire = fFire*mult;
        result.fCold = fCold*mult;
        result.fLightning = fLightning*mult;
        result.fPoisonPerFrame = fPoisonPerFrame*mult;
        result.fPoisonFrameLength = fPoisonFrameLength;
        return result;
    }

    eDamage operator*(const eDamage& mult) const {
        eDamage result;
        result.fPhysical = fPhysical*mult.fPhysical;
        result.fFire = fFire*mult.fFire;
        result.fCold = fCold*mult.fCold;
        result.fLightning = fLightning*mult.fLightning;
        result.fPoisonPerFrame = fPoisonPerFrame*mult.fPoisonPerFrame;
        result.fPoisonFrameLength = fPoisonFrameLength;
        return result;
    }

    static eDamage sRandom(const eDamage& min,
                           const eDamage& max);

    void clamp() {
        fPhysical = std::max(0.f, fPhysical);
        fFire = std::max(0.f, fFire);
        fCold = std::max(0.f, fCold);
        fLightning = std::max(0.f, fLightning);
        fPoisonPerFrame = std::max(0.f, fPoisonPerFrame);
        if(fPoisonPerFrame == 0.f) fPoisonFrameLength = 0.f;
    }
};

#endif // EDAMAGE_H

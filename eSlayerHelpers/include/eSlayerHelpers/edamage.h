#ifndef EDAMAGE_H
#define EDAMAGE_H

#include "eslayerhelpersexport.h"

class ePacket;

struct ESLAYERHELPERS_API eDamage {
    float fPhysical = 0.f;
    float fFire = 0.f;
    float fCold = 0.f;
    float fLightning = 0.f;

    eDamage operator/(const float div) const {
        eDamage result;
        result.fPhysical = fPhysical/div;
        result.fFire = fFire/div;
        result.fCold = fCold/div;
        result.fLightning = fLightning/div;
        return result;
    }

    eDamage operator*(const eDamage& mult) const {
        eDamage result;
        result.fPhysical = fPhysical*mult.fPhysical;
        result.fFire = fFire*mult.fFire;
        result.fCold = fCold*mult.fCold;
        result.fLightning = fLightning*mult.fLightning;
        return result;
    }

    static eDamage sRandom(const eDamage& min,
                           const eDamage& max);

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EDAMAGE_H

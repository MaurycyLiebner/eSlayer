#ifndef EDAMAGE_H
#define EDAMAGE_H

#include "eslayerhelpersexport.h"

class ePacket;

struct ESLAYERHELPERS_API eDamage {
    float fPhysical = 0.f;
    float fFire = 0.f;
    float fCold = 0.f;
    float fLightning = 0.f;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EDAMAGE_H

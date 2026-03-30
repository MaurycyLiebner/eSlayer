#ifndef EATTRIBUTES_H
#define EATTRIBUTES_H

#include "eslayerhelpersexport.h"

class ePacket;

struct ESLAYERHELPERS_API eAttributes {
    float fLevel = 1.f;
    float fStrength = 10.f;
    float fDexterity = 10.f;
    float fVitality = 10.f;
    float fEnergy = 10.f;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EATTRIBUTES_H

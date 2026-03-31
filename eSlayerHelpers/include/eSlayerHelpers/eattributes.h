#ifndef EATTRIBUTES_H
#define EATTRIBUTES_H

#include "eslayerhelpersexport.h"

class ePacket;

struct ESLAYERHELPERS_API eAttributes {
    float fLevel = 1.f;
    float fStrength = 20.f;
    float fDexterity = 20.f;
    float fVitality = 20.f;
    float fEnergy = 15.f;

    float nextLevelExp() const;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EATTRIBUTES_H

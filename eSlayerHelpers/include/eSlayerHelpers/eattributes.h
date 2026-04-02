#ifndef EATTRIBUTES_H
#define EATTRIBUTES_H

#include "eslayerhelpersexport.h"

#include <cstdint>
#include <vector>

class ePacket;

struct ESLAYERHELPERS_API eAttributes {
    uint8_t fLevel = 1;
    float fExp = 0.f;
    uint16_t fStrength = 20;
    uint16_t fDexterity = 20;
    uint16_t fVitality = 20;
    uint16_t fEnergy = 15;

    uint32_t nextLevelExp() const;

    void read(ePacket& p);
    void write(ePacket& p) const;

    static void load();
private:
    static uint8_t sMaxLevel;
    static std::vector<uint32_t> sLevelExperience;
};

#endif // EATTRIBUTES_H

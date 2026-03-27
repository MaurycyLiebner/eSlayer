#ifndef EMODIFIER_H
#define EMODIFIER_H

#include "eslayerhelpersexport.h"

#include <cstdint>

class ePacket;

enum class eModifierType : uint8_t {
    none,
    walkRun,
    attackSpeed,
    castRate,
    defenseValue,
    defensePercent,
    damageValue,
    damagePercent,
    damageFire,
    damageLightning,
    damageCold,
    attackRatingValue,
    attackRatingPercent,
    blockChancePercent,
    blockRecoverySpeed,
    hitRecoverySpeed,
    lifeValue,
    lifePercent,
    manaValue,
    manaPercent,
    pierceChance
};

struct ESLAYERHELPERS_API eModifier {
    eModifierType fType = eModifierType::none;
    float fValue1 = 0.f; // base / min
    float fValue2 = 0.f; // max

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EMODIFIER_H

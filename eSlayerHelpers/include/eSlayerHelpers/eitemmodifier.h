#ifndef EITEMMODIFIER_H
#define EITEMMODIFIER_H

#include "eslayerhelpersexport.h"

#include <cstdint>

class ePacket;

enum class eItemModifierType : uint8_t {
    walkRun,
    attackSpeed,
    castRate,
    defenseValue,
    defensePercent,
    damageMinValue,
    damageMaxValue,
    damageValue,
    damageMinPercent,
    damageMaxPercent,
    damagePercent,
    attackRatingValue,
    attackRatingPercent,
    blockChancePercent,
    blockRecoverySpeed,
    hitRecoverySpeed,
    lifeValue,
    lifePercent,
    manaValue,
    manaPercent
};

struct ESLAYERHELPERS_API eItemModifier {
    eItemModifierType fType;
    float fValue1; // base / min
    float fValue2; // max

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EITEMMODIFIER_H

#ifndef EMODIFIER_H
#define EMODIFIER_H

#include "eslayerhelpersexport.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include <cstdint>
#include <string>

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
    damagePoison,

    attackRatingValue,
    attackRatingPercent,

    blockChancePercent,
    blockRecoverySpeed,
    hitRecoverySpeed,

    lifeValue,
    lifePercent,
    manaValue,
    manaPercent,

    pierceChance,

    fireResistance,
    coldResistance,
    lightningResitance,
    poisonResistance,
    physicalResistance,

    maxFireResistance,
    maxColdResistance,
    maxLightningResitance,
    maxPoisonResistance,
    maxPhysicalResistance,

    strength,
    dexterity,
    energy,
    vitality,

    lifeSteal,
    manaSteal,

    meeleSplashDamage,
    knockback,

    allSkills,

    replenishLife,
    regenerateMana,

    fireSkillDamage,
    coldSkillDamage,
    lightningSkillDamage,
    poisonSkillDamage,

    count
};

struct ESLAYERHELPERS_API eModifier {
    eModifierType fType = eModifierType::none;
    float fValue1 = 0.f; // base / min
    float fValue2 = 0.f; // max

    int valuesUsed() const;
    void typeFromKey(const std::string& key);
    std::string typeName() const;
    void read(const std::string& key,
              const json& value);
    void read(const std::string& key,
              const float value);
    void read(const std::string& key,
              const float value1,
              const float value2);

    void read(ePacket& p);
    void write(ePacket& p) const;
};

namespace eModifierHelpers {
    ESLAYERHELPERS_API
    bool isPercent(const eModifierType type);
    ESLAYERHELPERS_API
    float clampValue(const float value,
                     const eModifierType type);
}

#endif // EMODIFIER_H

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

    coldLength,
    freezeLength,

    onAttack,
    onStriking,
    onKill,
    onStruck,
    onDeath,

    explode,

    skillLevel,

    count
};

enum class eModValuesUsage {
    none = 0,
    value1 = 1,
    value2 = 2,
    skillId = 4
};

inline eModValuesUsage operator|(const eModValuesUsage a, const eModValuesUsage b) {
    return static_cast<eModValuesUsage>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(const eModValuesUsage a, const eModValuesUsage b) {
    return static_cast<bool>(static_cast<int>(a) & static_cast<int>(b));
}

struct ESLAYERHELPERS_API eModifier {
    eModifierType fType = eModifierType::none;
    uint16_t fValue1 = 0; // base / min / chance
    uint16_t fValue2 = 0; // max / level

    uint16_t fSkillId = 0; // skill id

    eModValuesUsage valuesUsed() const;
    void typeFromKey(const std::string& key);
    std::string typeName() const;
    void read(const std::string& key,
              const json& value);

    std::string value1Name() const;
    std::string value2Name() const;
    std::string skillName() const;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

namespace eModifierHelpers {
    ESLAYERHELPERS_API
    bool isPercent(const eModValuesUsage used,
                   const eModifierType type);
    ESLAYERHELPERS_API
    int clampValue(const int value,
                   const eModifierType type);
}

#endif // EMODIFIER_H

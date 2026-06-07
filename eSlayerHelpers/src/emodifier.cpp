#include "eSlayerHelpers/emodifier.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/eskills.h"

std::map<eModifierType, std::string>
gModifierTypeToString = {
    { eModifierType::walkRun, "walkRun" },

    { eModifierType::attackSpeed, "attackSpeed" },
    { eModifierType::castRate, "castRate" },

    { eModifierType::defenseValue, "defenseValue" },
    { eModifierType::defensePercent, "defensePercent" },

    { eModifierType::damageValue, "damageValue" },
    { eModifierType::damagePercent, "damagePercent" },

    { eModifierType::damageFire, "damageFire" },
    { eModifierType::damageLightning, "damageLightning" },
    { eModifierType::damageCold, "damageCold" },
    { eModifierType::damagePoison, "damagePoison" },

    { eModifierType::attackRatingValue, "attackRatingValue" },
    { eModifierType::attackRatingPercent, "attackRatingPercent" },

    { eModifierType::blockChancePercent, "blockChancePercent" },
    { eModifierType::blockRecoverySpeed, "blockRecoverySpeed" },
    { eModifierType::hitRecoverySpeed, "hitRecoverySpeed" },

    { eModifierType::lifeValue, "lifeValue" },
    { eModifierType::lifePercent, "lifePercent" },
    { eModifierType::manaValue, "manaValue" },
    { eModifierType::manaPercent, "manaPercent" },

    { eModifierType::pierceChance, "pierceChance" },

    { eModifierType::fireResistance, "fireResistance" },
    { eModifierType::coldResistance, "coldResistance" },
    { eModifierType::lightningResistance, "lightningResistance" },
    { eModifierType::poisonResistance, "poisonResistance" },
    { eModifierType::physicalResistance, "physicalResistance" },

    { eModifierType::maxFireResistance, "maxFireResistance" },
    { eModifierType::maxColdResistance, "maxColdResistance" },
    { eModifierType::maxLightningResistance, "maxLightningResistance" },
    { eModifierType::maxPoisonResistance, "maxPoisonResistance" },
    { eModifierType::maxPhysicalResistance, "maxPhysicalResistance" },

    { eModifierType::strength, "strength" },
    { eModifierType::dexterity, "dexterity" },
    { eModifierType::energy, "energy" },
    { eModifierType::vitality, "vitality" },

    { eModifierType::lifeSteal, "lifeSteal" },
    { eModifierType::manaSteal, "manaSteal" },

    { eModifierType::meeleSplashDamage, "meeleSplashDamage" },
    { eModifierType::knockback, "knockback" },

    { eModifierType::allSkills, "allSkills" },

    { eModifierType::replenishLife, "replenishLife" },
    { eModifierType::regenerateMana, "regenerateMana" },

    { eModifierType::fireSkillDamage, "fireSkillDamage" },
    { eModifierType::coldSkillDamage, "coldSkillDamage" },
    { eModifierType::lightningSkillDamage, "lightningSkillDamage" },
    { eModifierType::poisonSkillDamage, "poisonSkillDamage" },

    { eModifierType::coldLength, "coldLength" },
    { eModifierType::freezeLength, "freezeLength" },

    { eModifierType::onAttack, "onAttack" },
    { eModifierType::onStriking, "onStriking" },
    { eModifierType::onKill, "onKill" },
    { eModifierType::onStruck, "onStruck" },
    { eModifierType::onDeath, "onDeath" },

    { eModifierType::fleshExplode, "fleshExplode" },
    { eModifierType::iceExplode, "iceExplode" },

    { eModifierType::skillLevel, "skillLevel" },

    { eModifierType::aura, "aura" },

    { eModifierType::dealsFireDamage, "dealsFireDamage" },
    { eModifierType::dealsColdDamage, "dealsColdDamage" },
    { eModifierType::dealsLightningDamage, "dealsLightningDamage" },
    { eModifierType::dealsPoisonDamage, "dealsPoisonDamage" },
    { eModifierType::dealsPhysicalDamage, "dealsPhysicalDamage" },

    { eModifierType::manaBurn, "manaBurn" },
    { eModifierType::multiShot, "multiShot" }
};

std::map<std::string, eModifierType>
gStringToModifierType = [] {
    std::map<std::string, eModifierType> m;
    for(const auto& [key, value] : gModifierTypeToString) {
        m[value] = key;
    }
    return m;
}();

eModValuesUsage eModifier::valuesUsed() const {
    switch(fType) {
    case eModifierType::none:
    case eModifierType::count:
        return eModValuesUsage::none;

    case eModifierType::damagePercent:
    case eModifierType::damageValue:
    case eModifierType::damageFire:
    case eModifierType::damageCold:
    case eModifierType::damageLightning:
    case eModifierType::damagePoison:

    case eModifierType::dealsFireDamage:
    case eModifierType::dealsColdDamage:
    case eModifierType::dealsLightningDamage:
    case eModifierType::dealsPoisonDamage:
    case eModifierType::dealsPhysicalDamage:
        return eModValuesUsage::value1 |
               eModValuesUsage::value2;

    case eModifierType::walkRun:
    case eModifierType::attackSpeed:
    case eModifierType::castRate:

    case eModifierType::defenseValue:
    case eModifierType::defensePercent:

    case eModifierType::attackRatingValue:
    case eModifierType::attackRatingPercent:

    case eModifierType::blockChancePercent:
    case eModifierType::blockRecoverySpeed:
    case eModifierType::hitRecoverySpeed:

    case eModifierType::lifeValue:
    case eModifierType::lifePercent:
    case eModifierType::manaValue:
    case eModifierType::manaPercent:

    case eModifierType::pierceChance:

    case eModifierType::fireResistance:
    case eModifierType::coldResistance:
    case eModifierType::lightningResistance:
    case eModifierType::poisonResistance:
    case eModifierType::physicalResistance:

    case eModifierType::maxFireResistance:
    case eModifierType::maxColdResistance:
    case eModifierType::maxLightningResistance:
    case eModifierType::maxPoisonResistance:
    case eModifierType::maxPhysicalResistance:

    case eModifierType::strength:
    case eModifierType::dexterity:
    case eModifierType::energy:
    case eModifierType::vitality:

    case eModifierType::lifeSteal:
    case eModifierType::manaSteal:

    case eModifierType::meeleSplashDamage:

    case eModifierType::allSkills:

    case eModifierType::replenishLife:
    case eModifierType::regenerateMana:

    case eModifierType::fireSkillDamage:
    case eModifierType::coldSkillDamage:
    case eModifierType::lightningSkillDamage:
    case eModifierType::poisonSkillDamage:

    case eModifierType::coldLength:
    case eModifierType::freezeLength:

    case eModifierType::skillLevel:

    case eModifierType::manaBurn:
    case eModifierType::multiShot:
        return eModValuesUsage::value1;

    case eModifierType::knockback:
    case eModifierType::fleshExplode:
    case eModifierType::iceExplode:
        return eModValuesUsage::none;

    case eModifierType::onAttack:
    case eModifierType::onStriking:
    case eModifierType::onKill:
    case eModifierType::onStruck:
    case eModifierType::onDeath:
        return eModValuesUsage::value1 |
               eModValuesUsage::value2 |
               eModValuesUsage::skillId;

    case eModifierType::aura:
        return eModValuesUsage::value2 |
               eModValuesUsage::skillId;
    }
    return eModValuesUsage::none;
}

void eModifier::typeFromKey(const std::string& key) {
    const auto it = gStringToModifierType.find(key);
    if(it == gStringToModifierType.end()) {
        eRuntimeThrow("Unrecognized modifier \"" + key + "\".");
    }
    fType = it->second;
}

std::string eModifier::typeName() const {
    return gModifierTypeToString[fType];
}

std::string eModifier::value1Name() const {
    switch(fType) {
    case eModifierType::none:
    case eModifierType::count:

    case eModifierType::aura:

    case eModifierType::knockback:
    case eModifierType::fleshExplode:
    case eModifierType::iceExplode:
        return "";
    case eModifierType::damagePercent:
    case eModifierType::damageValue:
    case eModifierType::damageFire:
    case eModifierType::damageCold:
    case eModifierType::damageLightning:

    case eModifierType::dealsFireDamage:
    case eModifierType::dealsColdDamage:
    case eModifierType::dealsLightningDamage:
    case eModifierType::dealsPoisonDamage:
    case eModifierType::dealsPhysicalDamage:
        return "min";
    case eModifierType::damagePoison:
        return "damage";
    case eModifierType::walkRun:
    case eModifierType::attackSpeed:
    case eModifierType::castRate:

    case eModifierType::defenseValue:
    case eModifierType::defensePercent:

    case eModifierType::attackRatingValue:
    case eModifierType::attackRatingPercent:

    case eModifierType::blockChancePercent:
    case eModifierType::blockRecoverySpeed:
    case eModifierType::hitRecoverySpeed:

    case eModifierType::lifeValue:
    case eModifierType::lifePercent:
    case eModifierType::manaValue:
    case eModifierType::manaPercent:

    case eModifierType::pierceChance:

    case eModifierType::fireResistance:
    case eModifierType::coldResistance:
    case eModifierType::lightningResistance:
    case eModifierType::poisonResistance:
    case eModifierType::physicalResistance:

    case eModifierType::maxFireResistance:
    case eModifierType::maxColdResistance:
    case eModifierType::maxLightningResistance:
    case eModifierType::maxPoisonResistance:
    case eModifierType::maxPhysicalResistance:

    case eModifierType::strength:
    case eModifierType::dexterity:
    case eModifierType::energy:
    case eModifierType::vitality:

    case eModifierType::lifeSteal:
    case eModifierType::manaSteal:

    case eModifierType::meeleSplashDamage:

    case eModifierType::allSkills:

    case eModifierType::replenishLife:
    case eModifierType::regenerateMana:

    case eModifierType::fireSkillDamage:
    case eModifierType::coldSkillDamage:
    case eModifierType::lightningSkillDamage:
    case eModifierType::poisonSkillDamage:

    case eModifierType::skillLevel:

    case eModifierType::manaBurn:
    case eModifierType::multiShot:
        return "value";

    case eModifierType::coldLength:
    case eModifierType::freezeLength:
        return "length";
    case eModifierType::onAttack:
    case eModifierType::onStriking:
    case eModifierType::onKill:
    case eModifierType::onStruck:
    case eModifierType::onDeath:
        return "chance";
    }
    return "";
}

std::string eModifier::value2Name() const {
    switch(fType) {
    case eModifierType::none:
    case eModifierType::count:
        return "";
    case eModifierType::damagePercent:
    case eModifierType::damageValue:
    case eModifierType::damageFire:
    case eModifierType::damageCold:
    case eModifierType::damageLightning:

    case eModifierType::dealsFireDamage:
    case eModifierType::dealsColdDamage:
    case eModifierType::dealsLightningDamage:
    case eModifierType::dealsPoisonDamage:
    case eModifierType::dealsPhysicalDamage:
        return "max";
    case eModifierType::damagePoison:
        return "duration";
    case eModifierType::walkRun:
    case eModifierType::attackSpeed:
    case eModifierType::castRate:

    case eModifierType::defenseValue:
    case eModifierType::defensePercent:

    case eModifierType::attackRatingValue:
    case eModifierType::attackRatingPercent:

    case eModifierType::blockChancePercent:
    case eModifierType::blockRecoverySpeed:
    case eModifierType::hitRecoverySpeed:

    case eModifierType::lifeValue:
    case eModifierType::lifePercent:
    case eModifierType::manaValue:
    case eModifierType::manaPercent:

    case eModifierType::pierceChance:

    case eModifierType::fireResistance:
    case eModifierType::coldResistance:
    case eModifierType::lightningResistance:
    case eModifierType::poisonResistance:
    case eModifierType::physicalResistance:

    case eModifierType::maxFireResistance:
    case eModifierType::maxColdResistance:
    case eModifierType::maxLightningResistance:
    case eModifierType::maxPoisonResistance:
    case eModifierType::maxPhysicalResistance:

    case eModifierType::strength:
    case eModifierType::dexterity:
    case eModifierType::energy:
    case eModifierType::vitality:

    case eModifierType::lifeSteal:
    case eModifierType::manaSteal:

    case eModifierType::meeleSplashDamage:
    case eModifierType::knockback:

    case eModifierType::allSkills:

    case eModifierType::replenishLife:
    case eModifierType::regenerateMana:

    case eModifierType::fireSkillDamage:
    case eModifierType::coldSkillDamage:
    case eModifierType::lightningSkillDamage:
    case eModifierType::poisonSkillDamage:

    case eModifierType::coldLength:
    case eModifierType::freezeLength:

    case eModifierType::fleshExplode:
    case eModifierType::iceExplode:

    case eModifierType::skillLevel:

    case eModifierType::manaBurn:
    case eModifierType::multiShot:
        return "";
    case eModifierType::onAttack:
    case eModifierType::onStriking:
    case eModifierType::onKill:
    case eModifierType::onStruck:
    case eModifierType::onDeath:

    case eModifierType::aura:
        return "level";
    }
    return "";
}

std::string eModifier::skillName() const {
    return "skill";
}

void eModifier::read(const std::string& key,
                     const json& value) {
    typeFromKey(key);
    const auto used = valuesUsed();

    const bool isInt = value.is_number_integer();
    const bool isArray = value.is_array();
    const bool isObj = value.is_object();

    if(isInt) {
        fValue1 = value;
        fValue2 = fValue1;
        if(used & eModValuesUsage::skillId) {
            eRuntimeThrow("No skill id provided, but needed.");
        }
    } else if(isArray) {
        const int size = value.size();
        if(size < 2) {
            eRuntimeThrow("Modifier size to small (should be 2 is " +
                          std::to_string(size) + ").");
        }
        fValue1 = value[0];
        fValue2 = value[1];
        if(used & eModValuesUsage::skillId) {
            eRuntimeThrow("No skill id provided, but needed.");
        }
    } else if(isObj) {
        if(used & eModValuesUsage::value1) {
            const auto v1Name = value1Name();
            fValue1 = value.value(v1Name, 0);
        }
        if(used & eModValuesUsage::value2) {
            const auto v2Name = value2Name();
            fValue2 = value.value(v2Name, 0);
        }
        if(used & eModValuesUsage::skillId) {
            const auto siName = skillName();
            const auto skillName = value.value(siName, "");
            fSkillId = eSkills::sSkills.id(skillName);
        }
    }
}

void eModifier::read(ePacket& p) {
    p >> fType;
    const auto used = valuesUsed();
    if(used & eModValuesUsage::value1) {
        p >> fValue1;
    }
    if(used & eModValuesUsage::value2) {
        p >> fValue2;
    }
    if(used & eModValuesUsage::skillId) {
        p >> fSkillId;
    }
}

void eModifier::write(ePacket& p) const {
    p << fType;
    const auto used = valuesUsed();
    if(used & eModValuesUsage::value1) {
        p << fValue1;
    }
    if(used & eModValuesUsage::value2) {
        p << fValue2;
    }
    if(used & eModValuesUsage::skillId) {
        p << fSkillId;
    }
}

bool eModifierHelpers::isPercent(
    const eModValuesUsage used,
    const eModifierType type) {
    switch(type) {
    case eModifierType::walkRun:

    case eModifierType::attackSpeed:
    case eModifierType::castRate:

    case eModifierType::defensePercent:
    case eModifierType::damagePercent:
    case eModifierType::attackRatingPercent:
    case eModifierType::blockChancePercent:
    case eModifierType::blockRecoverySpeed:
    case eModifierType::hitRecoverySpeed:

    case eModifierType::lifePercent:
    case eModifierType::manaPercent:

    case eModifierType::pierceChance:

    case eModifierType::fireResistance:
    case eModifierType::coldResistance:
    case eModifierType::lightningResistance:
    case eModifierType::poisonResistance:
    case eModifierType::physicalResistance:

    case eModifierType::maxFireResistance:
    case eModifierType::maxColdResistance:
    case eModifierType::maxLightningResistance:
    case eModifierType::maxPoisonResistance:
    case eModifierType::maxPhysicalResistance:

    case eModifierType::lifeSteal:
    case eModifierType::manaSteal:

    case eModifierType::meeleSplashDamage:
    case eModifierType::knockback:

    case eModifierType::fireSkillDamage:
    case eModifierType::coldSkillDamage:
    case eModifierType::lightningSkillDamage:
    case eModifierType::poisonSkillDamage:

    case eModifierType::manaBurn:
        return true;

    case eModifierType::none:
    case eModifierType::count:

    case eModifierType::defenseValue:
    case eModifierType::damageValue:

    case eModifierType::damageFire:
    case eModifierType::damageLightning:
    case eModifierType::damageCold:
    case eModifierType::damagePoison:

    case eModifierType::attackRatingValue:

    case eModifierType::lifeValue:
    case eModifierType::manaValue:

    case eModifierType::strength:
    case eModifierType::dexterity:
    case eModifierType::vitality:
    case eModifierType::energy:

    case eModifierType::allSkills:

    case eModifierType::replenishLife:
    case eModifierType::regenerateMana:

    case eModifierType::coldLength:
    case eModifierType::freezeLength:

    case eModifierType::fleshExplode:
    case eModifierType::iceExplode:

    case eModifierType::skillLevel:

    case eModifierType::aura:

    case eModifierType::dealsFireDamage:
    case eModifierType::dealsColdDamage:
    case eModifierType::dealsLightningDamage:
    case eModifierType::dealsPoisonDamage:
    case eModifierType::dealsPhysicalDamage:

    case eModifierType::multiShot:
        return false;
    case eModifierType::onAttack:
    case eModifierType::onStriking:
    case eModifierType::onKill:
    case eModifierType::onStruck:
    case eModifierType::onDeath:
        return used == eModValuesUsage::value1;
    }

    return false;
}

int eModifierHelpers::clampValue(
    const int value, const eModifierType type) {
    return std::clamp(value, 1, int(std::numeric_limits<uint16_t>::max()));
}

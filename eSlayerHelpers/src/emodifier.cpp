#include "eSlayerHelpers/emodifier.h"

#include "eSlayerHelpers/epacket.h"

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
    { eModifierType::lightningResitance, "lightningResitance" },
    { eModifierType::poisonResistance, "poisonResistance" },

    { eModifierType::maxFireResistance, "maxFireResistance" },
    { eModifierType::maxColdResistance, "maxColdResistance" },
    { eModifierType::maxLightningResitance, "maxLightningResitance" },
    { eModifierType::maxPoisonResistance, "maxPoisonResistance" },

    { eModifierType::strength, "strength" },
    { eModifierType::dexterity, "dexterity" },
    { eModifierType::energy, "energy" },
    { eModifierType::vitality, "vitality" },

    { eModifierType::lifeSteal, "lifeSteal" },
    { eModifierType::manaSteal, "manaSteal" },

    { eModifierType::meeleSplashDamage, "meeleSplashDamage" },
    { eModifierType::knockback, "knockback" }
};

std::map<std::string, eModifierType>
gStringToModifierType = [] {
    std::map<std::string, eModifierType> m;
    for(const auto& [key, value] : gModifierTypeToString) {
        m[value] = key;
    }
    return m;
}();

int eModifier::valuesUsed() const {
    switch(fType) {
    case eModifierType::none:
        return 0;
    case eModifierType::damagePercent:
    case eModifierType::damageValue:
    case eModifierType::damageFire:
    case eModifierType::damageCold:
    case eModifierType::damageLightning:
    case eModifierType::damagePoison:
        return 2;
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
    case eModifierType::lightningResitance:
    case eModifierType::poisonResistance:

    case eModifierType::maxFireResistance:
    case eModifierType::maxColdResistance:
    case eModifierType::maxLightningResitance:
    case eModifierType::maxPoisonResistance:

    case eModifierType::strength:
    case eModifierType::dexterity:
    case eModifierType::energy:
    case eModifierType::vitality:

    case eModifierType::lifeSteal:
    case eModifierType::manaSteal:

    case eModifierType::meeleSplashDamage:
    case eModifierType::knockback:
        return 1;
    }
    return 0;
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

void eModifier::read(const std::string& key,
                     const json& value) {
    typeFromKey(key);
    const int used = valuesUsed();
    if(used == 1) {
        fValue1 = value;
        fValue2 = fValue1;
    } else if(used == 2) {
        if(value.size() == 1) {
            fValue1 = value;
            fValue2 = fValue1;
        } else {
            fValue1 = value[0];
            fValue2 = value[1];
        }
    }
}

void eModifier::read(const std::string& key,
                     const float value) {
    typeFromKey(key);
    fValue1 = value;
    fValue2 = value;
}

void eModifier::read(const std::string& key,
                     const float value1,
                     const float value2) {
    typeFromKey(key);
    fValue1 = value1;
    fValue2 = value2;
}

void eModifier::read(ePacket& p) {
    p >> fType;
    p >> fValue1;
    p >> fValue2;
}

void eModifier::write(ePacket& p) const {
    p << fType;
    p << fValue1;
    p << fValue2;
}

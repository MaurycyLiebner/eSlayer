#include "eitemgenerator.h"

#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/erand.h>

uint32_t eItemGenerator::sNextItemId = 1;

void eItemGenerator::applyItemId(eItem& item) {
    item.fItemId = sNextItemId++;
}

std::vector<eModifierType> itemTypeMods(
    const eItemType type) {
    switch(type) {
    case eItemType::boots: {
        return {
            eModifierType::walkRun,

            // eModifierType::attackSpeed,
            // eModifierType::castRate,

            eModifierType::defenseValue,
            eModifierType::defensePercent,

            eModifierType::damageValue,
            eModifierType::damagePercent,

            // eModifierType::damageFire,
            // eModifierType::damageLightning,
            // eModifierType::damageCold,
            // eModifierType::damagePoison,

            // eModifierType::attackRatingValue,
            // eModifierType::attackRatingPercent,

            // eModifierType::blockChancePercent,
            // eModifierType::blockRecoverySpeed,
            eModifierType::hitRecoverySpeed,

            eModifierType::lifeValue,
            eModifierType::lifePercent,
            eModifierType::manaValue,
            eModifierType::manaPercent,

            // eModifierType::pierceChance,

            eModifierType::fireResistance,
            eModifierType::coldResistance,
            eModifierType::lightningResitance,
            eModifierType::poisonResistance,
            // eModifierType::physicalResistance,

            eModifierType::maxFireResistance,
            eModifierType::maxColdResistance,
            eModifierType::maxLightningResitance,
            eModifierType::maxPoisonResistance,
            // eModifierType::maxPhysicalResistance,

            eModifierType::strength,
            eModifierType::dexterity,
            eModifierType::energy,
            eModifierType::vitality,

            // eModifierType::lifeSteal,
            // eModifierType::manaSteal,

            // eModifierType::meeleSplashDamage,
            // eModifierType::knockback,

            // eModifierType::allSkills,

            eModifierType::replenishLife,
            eModifierType::regenerateMana,

            // eModifierType::fireSkillDamage,
            // eModifierType::coldSkillDamage,
            // eModifierType::lightningSkillDamage,
            // eModifierType::poisonSkillDamage
        };
    } break;
    case eItemType::gloves: {
        return {
            // eModifierType::walkRun,

            eModifierType::attackSpeed,
            eModifierType::castRate,

            eModifierType::defenseValue,
            eModifierType::defensePercent,

            eModifierType::damageValue,
            eModifierType::damagePercent,

            eModifierType::damageFire,
            eModifierType::damageLightning,
            eModifierType::damageCold,
            eModifierType::damagePoison,

            eModifierType::attackRatingValue,
            eModifierType::attackRatingPercent,

            // eModifierType::blockChancePercent,
            // eModifierType::blockRecoverySpeed,
            // eModifierType::hitRecoverySpeed,

            eModifierType::lifeValue,
            eModifierType::lifePercent,
            eModifierType::manaValue,
            eModifierType::manaPercent,

            eModifierType::pierceChance,

            eModifierType::fireResistance,
            eModifierType::coldResistance,
            eModifierType::lightningResitance,
            eModifierType::poisonResistance,
            // eModifierType::physicalResistance,

            eModifierType::maxFireResistance,
            eModifierType::maxColdResistance,
            eModifierType::maxLightningResitance,
            eModifierType::maxPoisonResistance,
            // eModifierType::maxPhysicalResistance,

            eModifierType::strength,
            eModifierType::dexterity,
            eModifierType::energy,
            eModifierType::vitality,

            eModifierType::lifeSteal,
            eModifierType::manaSteal,

            eModifierType::meeleSplashDamage,
            eModifierType::knockback,

            eModifierType::allSkills,

            eModifierType::replenishLife,
            eModifierType::regenerateMana,

            eModifierType::fireSkillDamage,
            eModifierType::coldSkillDamage,
            eModifierType::lightningSkillDamage,
            eModifierType::poisonSkillDamage
        };
    } break;
    case eItemType::helmet: {
        return {
            // eModifierType::walkRun,

            // eModifierType::attackSpeed,
            // eModifierType::castRate,

            eModifierType::defenseValue,
            eModifierType::defensePercent,

            // eModifierType::damageValue,
            // eModifierType::damagePercent,

            // eModifierType::damageFire,
            // eModifierType::damageLightning,
            // eModifierType::damageCold,
            // eModifierType::damagePoison,

            // eModifierType::attackRatingValue,
            // eModifierType::attackRatingPercent,

            // eModifierType::blockChancePercent,
            // eModifierType::blockRecoverySpeed,
            // eModifierType::hitRecoverySpeed,

            eModifierType::lifeValue,
            eModifierType::lifePercent,
            eModifierType::manaValue,
            eModifierType::manaPercent,

            // eModifierType::pierceChance,

            eModifierType::fireResistance,
            eModifierType::coldResistance,
            eModifierType::lightningResitance,
            eModifierType::poisonResistance,
            // eModifierType::physicalResistance,

            eModifierType::maxFireResistance,
            eModifierType::maxColdResistance,
            eModifierType::maxLightningResitance,
            eModifierType::maxPoisonResistance,
            // eModifierType::maxPhysicalResistance,

            eModifierType::strength,
            eModifierType::dexterity,
            eModifierType::energy,
            eModifierType::vitality,

            // eModifierType::lifeSteal,
            // eModifierType::manaSteal,

            // eModifierType::meeleSplashDamage,
            // eModifierType::knockback,

            eModifierType::allSkills,

            eModifierType::replenishLife,
            eModifierType::regenerateMana,

            eModifierType::fireSkillDamage,
            eModifierType::coldSkillDamage,
            eModifierType::lightningSkillDamage,
            eModifierType::poisonSkillDamage
        };
    } break;
    case eItemType::armor: {
        return {
            eModifierType::walkRun,

            // eModifierType::attackSpeed,
            // eModifierType::castRate,

            eModifierType::defenseValue,
            eModifierType::defensePercent,

            // eModifierType::damageValue,
            // eModifierType::damagePercent,

            // eModifierType::damageFire,
            // eModifierType::damageLightning,
            // eModifierType::damageCold,
            // eModifierType::damagePoison,

            // eModifierType::attackRatingValue,
            // eModifierType::attackRatingPercent,

            // eModifierType::blockChancePercent,
            // eModifierType::blockRecoverySpeed,
            eModifierType::hitRecoverySpeed,

            eModifierType::lifeValue,
            eModifierType::lifePercent,
            eModifierType::manaValue,
            eModifierType::manaPercent,

            // eModifierType::pierceChance,

            eModifierType::fireResistance,
            eModifierType::coldResistance,
            eModifierType::lightningResitance,
            eModifierType::poisonResistance,
            // eModifierType::physicalResistance,

            eModifierType::maxFireResistance,
            eModifierType::maxColdResistance,
            eModifierType::maxLightningResitance,
            eModifierType::maxPoisonResistance,
            // eModifierType::maxPhysicalResistance,

            eModifierType::strength,
            eModifierType::dexterity,
            eModifierType::energy,
            eModifierType::vitality,

            // eModifierType::lifeSteal,
            // eModifierType::manaSteal,

            // eModifierType::meeleSplashDamage,
            // eModifierType::knockback,

            eModifierType::allSkills,

            eModifierType::replenishLife,
            eModifierType::regenerateMana,

            eModifierType::fireSkillDamage,
            eModifierType::coldSkillDamage,
            eModifierType::lightningSkillDamage,
            eModifierType::poisonSkillDamage
        };
    } break;
    case eItemType::belt: {
        return {
            eModifierType::walkRun,

            // eModifierType::attackSpeed,
            // eModifierType::castRate,

            eModifierType::defenseValue,
            eModifierType::defensePercent,

            // eModifierType::damageValue,
            // eModifierType::damagePercent,

            // eModifierType::damageFire,
            // eModifierType::damageLightning,
            // eModifierType::damageCold,
            // eModifierType::damagePoison,

            // eModifierType::attackRatingValue,
            // eModifierType::attackRatingPercent,

            // eModifierType::blockChancePercent,
            // eModifierType::blockRecoverySpeed,
            eModifierType::hitRecoverySpeed,

            eModifierType::lifeValue,
            eModifierType::lifePercent,
            eModifierType::manaValue,
            eModifierType::manaPercent,

            // eModifierType::pierceChance,

            eModifierType::fireResistance,
            eModifierType::coldResistance,
            eModifierType::lightningResitance,
            eModifierType::poisonResistance,
            // eModifierType::physicalResistance,

            eModifierType::maxFireResistance,
            eModifierType::maxColdResistance,
            eModifierType::maxLightningResitance,
            eModifierType::maxPoisonResistance,
            // eModifierType::maxPhysicalResistance,

            eModifierType::strength,
            eModifierType::dexterity,
            eModifierType::energy,
            eModifierType::vitality,

            // eModifierType::lifeSteal,
            // eModifierType::manaSteal,

            // eModifierType::meeleSplashDamage,
            // eModifierType::knockback,

            eModifierType::allSkills,

            eModifierType::replenishLife,
            eModifierType::regenerateMana,

            // eModifierType::fireSkillDamage,
            // eModifierType::coldSkillDamage,
            // eModifierType::lightningSkillDamage,
            // eModifierType::poisonSkillDamage
        };
    } break;
    case eItemType::ring:
    case eItemType::amulet: {
        return {
            // eModifierType::walkRun,

            // eModifierType::attackSpeed,
            eModifierType::castRate,

            // eModifierType::defenseValue,
            // eModifierType::defensePercent,

            // eModifierType::damageValue,
            // eModifierType::damagePercent,

            // eModifierType::damageFire,
            // eModifierType::damageLightning,
            // eModifierType::damageCold,
            // eModifierType::damagePoison,

            eModifierType::attackRatingValue,
            eModifierType::attackRatingPercent,

            // eModifierType::blockChancePercent,
            // eModifierType::blockRecoverySpeed,
            // eModifierType::hitRecoverySpeed,

            eModifierType::lifeValue,
            eModifierType::lifePercent,
            eModifierType::manaValue,
            eModifierType::manaPercent,

            // eModifierType::pierceChance,

            eModifierType::fireResistance,
            eModifierType::coldResistance,
            eModifierType::lightningResitance,
            eModifierType::poisonResistance,
            // eModifierType::physicalResistance,

            eModifierType::maxFireResistance,
            eModifierType::maxColdResistance,
            eModifierType::maxLightningResitance,
            eModifierType::maxPoisonResistance,
            // eModifierType::maxPhysicalResistance,

            eModifierType::strength,
            eModifierType::dexterity,
            eModifierType::energy,
            eModifierType::vitality,

            eModifierType::lifeSteal,
            eModifierType::manaSteal,

            // eModifierType::meeleSplashDamage,
            // eModifierType::knockback,

            eModifierType::allSkills,

            eModifierType::replenishLife,
            eModifierType::regenerateMana,

            eModifierType::fireSkillDamage,
            eModifierType::coldSkillDamage,
            eModifierType::lightningSkillDamage,
            eModifierType::poisonSkillDamage
        };
    } break;
    case eItemType::weapon: {
        return {
            // eModifierType::walkRun,

            eModifierType::attackSpeed,
            eModifierType::castRate,

            // eModifierType::defenseValue,
            // eModifierType::defensePercent,

            eModifierType::damageValue,
            eModifierType::damagePercent,

            eModifierType::damageFire,
            eModifierType::damageLightning,
            eModifierType::damageCold,
            eModifierType::damagePoison,

            eModifierType::attackRatingValue,
            eModifierType::attackRatingPercent,

            // eModifierType::blockChancePercent,
            // eModifierType::blockRecoverySpeed,
            // eModifierType::hitRecoverySpeed,

            eModifierType::lifeValue,
            eModifierType::lifePercent,
            eModifierType::manaValue,
            eModifierType::manaPercent,

            eModifierType::pierceChance,

            eModifierType::fireResistance,
            eModifierType::coldResistance,
            eModifierType::lightningResitance,
            eModifierType::poisonResistance,
            // eModifierType::physicalResistance,

            eModifierType::maxFireResistance,
            eModifierType::maxColdResistance,
            eModifierType::maxLightningResitance,
            eModifierType::maxPoisonResistance,
            // eModifierType::maxPhysicalResistance,

            eModifierType::strength,
            eModifierType::dexterity,
            eModifierType::energy,
            eModifierType::vitality,

            eModifierType::lifeSteal,
            eModifierType::manaSteal,

            eModifierType::meeleSplashDamage,
            eModifierType::knockback,

            eModifierType::allSkills,

            eModifierType::replenishLife,
            eModifierType::regenerateMana,

            eModifierType::fireSkillDamage,
            eModifierType::coldSkillDamage,
            eModifierType::lightningSkillDamage,
            eModifierType::poisonSkillDamage
        };
    } break;
    case eItemType::shield: {
        return {
            // eModifierType::walkRun,

            // eModifierType::attackSpeed,
            // eModifierType::castRate,

            eModifierType::defenseValue,
            eModifierType::defensePercent,

            eModifierType::damageValue,
            eModifierType::damagePercent,

            // eModifierType::damageFire,
            // eModifierType::damageLightning,
            // eModifierType::damageCold,
            // eModifierType::damagePoison,

            // eModifierType::attackRatingValue,
            // eModifierType::attackRatingPercent,

            eModifierType::blockChancePercent,
            eModifierType::blockRecoverySpeed,
            eModifierType::hitRecoverySpeed,

            eModifierType::lifeValue,
            eModifierType::lifePercent,
            eModifierType::manaValue,
            eModifierType::manaPercent,

            // eModifierType::pierceChance,

            eModifierType::fireResistance,
            eModifierType::coldResistance,
            eModifierType::lightningResitance,
            eModifierType::poisonResistance,
            // eModifierType::physicalResistance,

            eModifierType::maxFireResistance,
            eModifierType::maxColdResistance,
            eModifierType::maxLightningResitance,
            eModifierType::maxPoisonResistance,
            // eModifierType::maxPhysicalResistance,

            eModifierType::strength,
            eModifierType::dexterity,
            eModifierType::energy,
            eModifierType::vitality,

            // eModifierType::lifeSteal,
            // eModifierType::manaSteal,

            // eModifierType::meeleSplashDamage,
            // eModifierType::knockback,

            eModifierType::allSkills,

            eModifierType::replenishLife,
            eModifierType::regenerateMana,

            eModifierType::fireSkillDamage,
            eModifierType::coldSkillDamage,
            eModifierType::lightningSkillDamage,
            eModifierType::poisonSkillDamage
        };
    } break;
    case eItemType::arrows:
    case eItemType::bolts: {
        return {
            // eModifierType::walkRun,

            eModifierType::attackSpeed,
            // eModifierType::castRate,

            // eModifierType::defenseValue,
            // eModifierType::defensePercent,

            eModifierType::damageValue,
            eModifierType::damagePercent,

            eModifierType::damageFire,
            eModifierType::damageLightning,
            eModifierType::damageCold,
            eModifierType::damagePoison,

            eModifierType::attackRatingValue,
            eModifierType::attackRatingPercent,

            // eModifierType::blockChancePercent,
            // eModifierType::blockRecoverySpeed,
            // eModifierType::hitRecoverySpeed,

            // eModifierType::lifeValue,
            // eModifierType::lifePercent,
            // eModifierType::manaValue,
            // eModifierType::manaPercent,

            eModifierType::pierceChance,

            // eModifierType::fireResistance,
            // eModifierType::coldResistance,
            // eModifierType::lightningResitance,
            // eModifierType::poisonResistance,

            // eModifierType::maxFireResistance,
            // eModifierType::maxColdResistance,
            // eModifierType::maxLightningResitance,
            // eModifierType::maxPoisonResistance,

            eModifierType::strength,
            eModifierType::dexterity,
            eModifierType::energy,
            eModifierType::vitality,

            eModifierType::lifeSteal,
            eModifierType::manaSteal,

            // eModifierType::meeleSplashDamage,
            eModifierType::knockback,

            eModifierType::allSkills,

            eModifierType::replenishLife,
            eModifierType::regenerateMana,

            eModifierType::fireSkillDamage,
            eModifierType::coldSkillDamage,
            eModifierType::lightningSkillDamage,
            eModifierType::poisonSkillDamage
        };
    } break;
    default:
        break;
    }
    return {};
}

eItem eItemGenerator::generateItem(
    const float level, const float worth) {
    eItem item;
    eItemGenerator::applyItemId(item);
    const int typeMax = eItemsData::sItems.size() - 1;
    const int typeId = eRand::rand(0, typeMax);
    const auto& itemData = eItemsData::get(typeId);
    const auto type = itemData.fType;
    item.fDataId = typeId;
    item.fType = type;
    item.fSubType = itemData.fSubtype;
    item.fRequiredLevel = level;

    if(type == eItemType::potion) return item;

    float minDmg = eRand::randF(itemData.fValue1Min,
                                itemData.fValue1Max);
    minDmg = eModifierHelpers::clampValue(
        minDmg, eModifierType::damageValue);
    float maxDmg = eRand::randF(std::max(minDmg, itemData.fValue2Min),
                                itemData.fValue2Max);
    maxDmg = eModifierHelpers::clampValue(
        maxDmg, eModifierType::damageValue);

    float def = eRand::randF(itemData.fValue3Min,
                             itemData.fValue3Max);
    def = eModifierHelpers::clampValue(
        def, eModifierType::defenseValue);
    float block = eRand::randF(itemData.fValue4Min,
                               itemData.fValue4Max);
    block = eModifierHelpers::clampValue(
        block, eModifierType::blockChancePercent);

    item.fMinDmg = minDmg;
    item.fMaxDmg = maxDmg;
    item.fDefense = def;
    item.fBlockChance = block;

    eItemRarity rarity;
    if(worth > 4.f) {
        item.fRarity = eItemRarity::rare;
    } else if(worth < 1.f) {
        item.fRarity = eItemRarity::normal;
        return item;
    } else {
        item.fRarity = eItemRarity::magic;
    }

    auto modTypes = itemTypeMods(type);
    auto& mods = item.fModifiers;
    const auto genModifier = [&](const float worth) {
        const int modIdMax = modTypes.size() - 1;
        const int modId = eRand::rand(0, modIdMax);
        const auto type = modTypes[modId];
        modTypes.erase(modTypes.begin() + modId);
        auto& mod = mods.emplace_back();
        mod.fType = type;
        switch(type) {
        case eModifierType::none:
        case eModifierType::count:
            break;
        case eModifierType::walkRun:
            mod.fValue1 = worth*30.f;
            break;
        case eModifierType::attackSpeed:
        case eModifierType::castRate:
            mod.fValue1 = worth*0.25f;
            break;

        case eModifierType::defenseValue:
            mod.fValue1 = level*worth*5.f;
            break;
        case eModifierType::defensePercent:
            mod.fValue1 = worth*1.5f;
            break;

        case eModifierType::damageValue:
            mod.fValue1 = level*worth*1.f;
            mod.fValue2 = level*worth*2.f;
            break;
        case eModifierType::damagePercent:
            mod.fValue1 = (5 + sqrt(level))*worth*0.05f;
            mod.fValue2 = (5 + sqrt(level))*worth*0.3f;
            break;

        case eModifierType::damageFire:
            mod.fValue1 = level*worth*1.f;
            mod.fValue2 = level*worth*2.f;
            break;
        case eModifierType::damageLightning:
            mod.fValue1 = 1.f;
            mod.fValue2 = level*worth*3.f;
            break;
        case eModifierType::damageCold:
            mod.fValue1 = level*worth*1.f;
            mod.fValue2 = level*worth*2.f;
            break;
        case eModifierType::damagePoison:
            mod.fValue1 = level*worth*2.f;
            mod.fValue2 = worth*5.f;
            break;

        case eModifierType::attackRatingValue:
            mod.fValue1 = (level + 5)*worth*10.f;
            break;
        case eModifierType::attackRatingPercent:
            mod.fValue1 = worth*0.5f;
            break;

        case eModifierType::blockChancePercent:
        case eModifierType::blockRecoverySpeed:
        case eModifierType::hitRecoverySpeed:
            mod.fValue1 = worth*0.25f;
            break;

        case eModifierType::lifeValue:
            mod.fValue1 = (level + 5)*worth;
            break;
        case eModifierType::lifePercent:
            mod.fValue1 = worth*0.1f;
            break;
        case eModifierType::manaValue:
            mod.fValue1 = (level + 5)*worth;
            break;
        case eModifierType::manaPercent:
            mod.fValue1 = worth*0.1f;
            break;

        case eModifierType::pierceChance:
            mod.fValue1 = worth*0.5f;
            break;

        case eModifierType::fireResistance:
        case eModifierType::coldResistance:
        case eModifierType::lightningResitance:
        case eModifierType::poisonResistance:
        case eModifierType::physicalResistance:
            mod.fValue1 = sqrt(sqrt(level))*worth*0.15f;
            break;

        case eModifierType::maxFireResistance:
        case eModifierType::maxColdResistance:
        case eModifierType::maxLightningResitance:
        case eModifierType::maxPoisonResistance:
        case eModifierType::maxPhysicalResistance:
            mod.fValue1 = worth*0.05f;
            break;

        case eModifierType::strength:
        case eModifierType::dexterity:
        case eModifierType::energy:
        case eModifierType::vitality:
            mod.fValue1 = sqrt(level)*worth*2;
            break;

        case eModifierType::lifeSteal:
        case eModifierType::manaSteal:
            mod.fValue1 = worth*0.05f;
            break;

        case eModifierType::meeleSplashDamage:
            mod.fValue1 = worth*0.5f;
            break;
        case eModifierType::knockback:
            break;

        case eModifierType::allSkills:
            mod.fValue1 = std::ceil(sqrt(sqrt(level))*worth);
            break;

        case eModifierType::replenishLife:
        case eModifierType::regenerateMana:
            mod.fValue1 = sqrt(level)*worth*2.f;
            break;

        case eModifierType::fireSkillDamage:
        case eModifierType::coldSkillDamage:
        case eModifierType::lightningSkillDamage:
        case eModifierType::poisonSkillDamage:
            mod.fValue1 = worth*0.15f;
            break;
        }
        mod.fValue1 = eModifierHelpers::clampValue(mod.fValue1, type);
        mod.fValue2 = eModifierHelpers::clampValue(mod.fValue2, type);
    };
    float remWorth = worth;
    const int maxMods = rarity == eItemRarity::rare ? 8 : 4;
    while(remWorth >= 0.25f && !modTypes.empty()) {
        const float maxWorth = std::clamp(remWorth, 0.25f, 1.f);
        const float mworth = eRand::randF(0.25f, maxWorth);
        genModifier(mworth);
        remWorth -= mworth;
        if(mods.size() >= maxMods) break;
    }
    return item;
}

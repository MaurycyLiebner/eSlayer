#include "eitemgenerator.h"

#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/eskills.h>

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
            // eModifierType::poisonSkillDamage,

            eModifierType::onStruck
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
            eModifierType::poisonSkillDamage,

            eModifierType::onStruck,
            eModifierType::onAttack,
            eModifierType::onKill,
            eModifierType::onStriking,
            eModifierType::onDeath
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
            eModifierType::poisonSkillDamage,

            eModifierType::onStruck
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
            eModifierType::poisonSkillDamage,

            eModifierType::onStruck
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

            eModifierType::onStruck,
            eModifierType::onAttack,
            eModifierType::onKill,
            eModifierType::onStriking
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
            eModifierType::poisonSkillDamage,

            eModifierType::onStriking,
            eModifierType::onAttack,
            eModifierType::onKill
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
            eModifierType::poisonSkillDamage,

            eModifierType::onStruck,
            eModifierType::onDeath
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

eItem eItemGenerator::generatePotion(
    const float level, const float worth) {
    eItem item;
    std::vector<int> typeIds;
    for(int i = 0; i < eItemsData::sItems.size(); i++) {
        const auto& itemData = eItemsData::get(i);
        if(itemData.fType != eItemType::potion) continue;
        typeIds.emplace_back(i);
    }
    if(typeIds.empty()) return item;
    const int id = eRand::rand() % typeIds.size();
    const int typeId = typeIds[id];
    const auto& itemData = eItemsData::get(typeId);
    const auto type = itemData.fType;
    item.fDataId = typeId;
    item.fType = type;
    item.fSubType = itemData.fSubtype;
    return item;
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

    int minDmg = eRand::rand(itemData.fValue1Min,
                             itemData.fValue1Max);
    int maxDmg = eRand::rand(itemData.fValue2Min,
                             itemData.fValue2Max);

    int def = eRand::rand(itemData.fValue3Min,
                          itemData.fValue3Max);
    int block = eRand::rand(itemData.fValue4Min,
                            itemData.fValue4Max);

    minDmg = eModifierHelpers::clampValue(
        minDmg, eModifierType::damageValue);
    maxDmg = eModifierHelpers::clampValue(
        maxDmg, eModifierType::damageValue);
    if(maxDmg < minDmg) maxDmg = minDmg;
    def = eModifierHelpers::clampValue(
        def, eModifierType::defenseValue);
    block = eModifierHelpers::clampValue(
        block, eModifierType::blockChancePercent);

    item.fMinDmg = minDmg;
    item.fMaxDmg = maxDmg;
    item.fDefense = def;
    item.fBlockChance = block;

    const bool canBeNormal = type != eItemType::ring &&
                             type != eItemType::amulet;

    if(worth > 4.f) {
        item.fRarity = eItemRarity::rare;
    } else if(canBeNormal && worth < 1.f) {
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
            mod.fValue1 = 30*worth;
            break;
        case eModifierType::attackSpeed:
        case eModifierType::castRate:
            mod.fValue1 = 25*worth;
            break;

        case eModifierType::defenseValue:
            mod.fValue1 = 5*level*worth;
            break;
        case eModifierType::defensePercent:
            mod.fValue1 = 150*worth;
            break;

        case eModifierType::damageValue:
            mod.fValue1 = 1*level*worth;
            mod.fValue2 = 2*level*worth;
            break;
        case eModifierType::damagePercent:
            mod.fValue1 = 5*(5 + sqrt(level))*worth;
            mod.fValue2 = 30*(5 + sqrt(level))*worth;
            break;

        case eModifierType::damageFire:
            mod.fValue1 = 1*level*worth;
            mod.fValue2 = 2*level*worth;
            break;
        case eModifierType::damageLightning:
            mod.fValue1 = 1;
            mod.fValue2 = 3*level*worth;
            break;
        case eModifierType::damageCold:
            mod.fValue1 = 1*level*worth;
            mod.fValue2 = 2*level*worth;
            break;
        case eModifierType::damagePoison:
            mod.fValue1 = 2*level*worth;
            mod.fValue2 = 5*worth;
            break;

        case eModifierType::attackRatingValue:
            mod.fValue1 = 10*(level + 5)*worth;
            break;
        case eModifierType::attackRatingPercent:
            mod.fValue1 = 50*worth;
            break;

        case eModifierType::blockChancePercent:
        case eModifierType::blockRecoverySpeed:
        case eModifierType::hitRecoverySpeed:
            mod.fValue1 = 25*worth;
            break;

        case eModifierType::lifeValue:
            mod.fValue1 = (level + 5)*worth;
            break;
        case eModifierType::lifePercent:
            mod.fValue1 = 10*worth;
            break;
        case eModifierType::manaValue:
            mod.fValue1 = (level + 5)*worth;
            break;
        case eModifierType::manaPercent:
            mod.fValue1 = 10*worth;
            break;

        case eModifierType::pierceChance:
            mod.fValue1 = 50*worth;
            break;

        case eModifierType::fireResistance:
        case eModifierType::coldResistance:
        case eModifierType::lightningResitance:
        case eModifierType::poisonResistance:
        case eModifierType::physicalResistance:
            mod.fValue1 = 15*sqrt(sqrt(level))*worth;
            break;

        case eModifierType::maxFireResistance:
        case eModifierType::maxColdResistance:
        case eModifierType::maxLightningResitance:
        case eModifierType::maxPoisonResistance:
        case eModifierType::maxPhysicalResistance:
            mod.fValue1 = 5*worth;
            break;

        case eModifierType::strength:
        case eModifierType::dexterity:
        case eModifierType::energy:
        case eModifierType::vitality:
            mod.fValue1 = 2*sqrt(level)*worth;
            break;

        case eModifierType::lifeSteal:
        case eModifierType::manaSteal:
            mod.fValue1 = 5*worth;
            break;

        case eModifierType::meeleSplashDamage:
            mod.fValue1 = 50*worth;
            break;
        case eModifierType::knockback:
        case eModifierType::fleshExplode:
        case eModifierType::iceExplode:

        case eModifierType::skillLevel:
            break;

        case eModifierType::allSkills:
            mod.fValue1 = std::ceil(sqrt(sqrt(level))*worth);
            break;

        case eModifierType::replenishLife:
        case eModifierType::regenerateMana:
            mod.fValue1 = 2*sqrt(level)*worth;
            break;

        case eModifierType::fireSkillDamage:
        case eModifierType::coldSkillDamage:
        case eModifierType::lightningSkillDamage:
        case eModifierType::poisonSkillDamage:
            mod.fValue1 = 15*worth;
            break;

        case eModifierType::coldLength:
            mod.fValue1 = 8*worth;
            break;
        case eModifierType::freezeLength:
            mod.fValue1 = 4*worth;
            break;

        case eModifierType::onAttack:
        case eModifierType::onKill:
        case eModifierType::onStriking:
            mod.fValue1 = 5*worth;
            mod.fValue2 = level;
            mod.fSkillId = eSkills::sSkills.id("frostNova");
            break;
        case eModifierType::onDeath:
            mod.fValue1 = 100*worth;
            mod.fValue2 = level;
            mod.fSkillId = eSkills::sSkills.id("frostNova");
            break;
        case eModifierType::onStruck:
            mod.fValue1 = 10*worth;
            mod.fValue2 = sqrt(level);
            mod.fSkillId = eSkills::sSkills.id("frostNova");
            break;
        }
        mod.fValue1 = eModifierHelpers::clampValue(mod.fValue1, type);
        mod.fValue2 = eModifierHelpers::clampValue(mod.fValue2, type);
    };
    float remWorth = worth;
    const int maxMods = item.fRarity == eItemRarity::rare ? 8 : 4;
    while(remWorth >= 0.25f && !modTypes.empty()) {
        const float maxWorth = std::clamp(remWorth, 0.25f, 1.f);
        const float mworth = eRand::randF(0.25f, maxWorth);
        genModifier(mworth);
        remWorth -= mworth;
        if(mods.size() >= maxMods) break;
    }
    return item;
}

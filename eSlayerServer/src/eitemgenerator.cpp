#include "eitemgenerator.h"

#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eitemaffixes.h>

uint32_t eItemGenerator::sNextItemId = 1;

void eItemGenerator::applyItemId(eItem& item) {
    item.fItemId = sNextItemId++;
}

eItem eItemGenerator::generatePotion(
    const int level, const float worth) {
    eItem item;
    eItemGenerator::applyItemId(item);
    std::vector<int> typeIds;
    for(int i = 0; i < eItemsData::sItems.size(); i++) {
        const auto& itemData = eItemsData::get(i);
        if(itemData.fType != eItemType::potion) continue;
        const int levelReq = itemData.fLevelReq;
        if(levelReq > level) continue;
        typeIds.emplace_back(i);
    }
    if(typeIds.empty()) return item;
    const int typeId = eRand::randomElement(typeIds);
    const auto& itemData = eItemsData::get(typeId);
    const auto type = itemData.fType;
    item.fDataId = typeId;
    item.fType = type;
    item.fSubType = itemData.fSubtype;
    return item;
}

eItem eItemGenerator::generateItem(
    const int typeId,
    const int level, const float worth) {
    eItem item;
    eItemGenerator::applyItemId(item);
    const auto& itemData = eItemsData::get(typeId);
    const auto type = itemData.fType;
    item.fDataId = typeId;
    item.fType = type;
    item.fSubType = itemData.fSubtype;
    if(type == eItemType::gold) {
        item.fCount = 1 + 5.f*sqrt(level)*worth;
        return item;
    } else if(type == eItemType::potion) {
        item.fRequiredLevel = 1;
        return item;
    } else {
        item.fRequiredLevel = level;
    }

    int minDmg = eRand::rand(itemData.fMinDamageMin,
                             itemData.fMinDamageMax);
    int maxDmg = eRand::rand(itemData.fMaxDamageMin,
                             itemData.fMaxDamageMax);

    int def = eRand::rand(itemData.fDefenseMin,
                          itemData.fDefenseMax);
    int block = eRand::rand(itemData.fBlockChanceMin,
                            itemData.fBlockChanceMax);

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

    auto& mods = item.fModifiers;
    float remWorth = worth;
    const int maxMods = item.fRarity == eItemRarity::rare ? 8 : 4;
    auto prefixOptions = eItemAffixes::sTypePrefixes[type];
    auto suffixOptions = eItemAffixes::sTypeSuffixes[type];
    while(remWorth >= 0.25f) {
        if(mods.size() >= maxMods) break;
        if(prefixOptions.empty() && suffixOptions.empty()) break;
        const bool suffix = !suffixOptions.empty() &&
                            (prefixOptions.empty() || eRand::randChance(0.5f));
        if(suffix) {
            const int oid = eRand::rand(0, suffixOptions.size() - 1);
            const int id = suffixOptions[oid];
            suffixOptions.erase(suffixOptions.begin() + oid);
            if(item.fSuffix == 0) item.fSuffix = id;
            const auto& mod = eItemAffixes::sSuffixes.get(id);
            mod.generate(level, remWorth, mods);
        } else {
            const int oid = eRand::rand(0, prefixOptions.size() - 1);
            const int id = prefixOptions[oid];
            prefixOptions.erase(prefixOptions.begin() + oid);
            if(item.fPrefix == 0) item.fPrefix = id;
            const auto& mod = eItemAffixes::sPrefixes.get(id);
            mod.generate(level, remWorth, mods);
        }
    }
    return item;
}

eItem eItemGenerator::generateItem(
    const int level, const float worth) {
    int typeId;
    if(worth < 1.f && eRand::randChance(0.5f)) {
        typeId = eItemsData::sGoldIds[0];
    } else {
        std::vector<int> typeIds;
        for(int i = 0; i < eItemsData::sItems.size(); i++) {
            const auto& itemData = eItemsData::get(i);
            const int levelReq = itemData.fLevelReq;
            if(levelReq > level) continue;
            typeIds.emplace_back(i);
        }
        if(typeIds.empty()) return eItem();
        typeId = eRand::randomElement(typeIds);
    }
    return generateItem(typeId, level, worth);
}

eItem eItemGenerator::generateGold(
    const uint32_t count) {
    eItem result;
    std::vector<int> typeIds;
    for(int i = 0; i < eItemsData::sItems.size(); i++) {
        const auto& itemData = eItemsData::get(i);
        const auto type = itemData.fType;
        if(type != eItemType::gold) continue;
        eItemGenerator::applyItemId(result);
        result.fDataId = i;
        result.fType = type;
        result.fSubType = itemData.fSubtype;
        result.fCount = count;
        break;
    }
    return result;
}

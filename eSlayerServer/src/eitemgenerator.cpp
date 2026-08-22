#include "eitemgenerator.h"

#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eitemaffixes.h>
#include <eSlayerHelpers/epotiontype.h>

uint32_t eItemGenerator::sNextItemId = 1;

void eItemGenerator::applyItemId(eItem& item) {
    item.fItemId = sNextItemId++;
}

eItem eItemGenerator::generatePotion(
    const int level, const float worth) {
    eItem item;
    eItemGenerator::applyItemId(item);
    std::vector<int> typeIds;
    for(const auto& type : ePotionTypes::sTypes) {
        const auto& vec = type.fValue;
        for(auto it = vec.rbegin(); it != vec.rend(); it++) {
            const int id = *it;
            const auto& item = eItemsData::get(id);
            if(item.fLevelReq <= level) {
                typeIds.emplace_back(id);
                break;
            }
        }
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
    if(type == eItemType::questItem) {
        item.fRarity = eItemRarity::quest;
        return item;
    } else if(type == eItemType::gold) {
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

    const auto extractLevelReq = [level, typeId](const bool suffix) {
        std::set<int> result;
        const auto& affixes = suffix ? eItemAffixes::sTypeSuffixes[typeId] :
            eItemAffixes::sTypePrefixes[typeId];
        const auto& mods = suffix ? eItemAffixes::sSuffixes :
            eItemAffixes::sPrefixes;
        for(const auto affix : affixes) {
            const auto& mod = mods.get(affix);
            if(level < mod.fLevelReq) continue;
            result.emplace(affix);
        }
        return result;
    };

    auto prefixOptions = extractLevelReq(false);
    auto suffixOptions = extractLevelReq(true);
    while(remWorth >= 0.25f) {
        if(mods.size() >= maxMods) break;
        if(prefixOptions.empty() && suffixOptions.empty()) break;
        const bool suffix = !suffixOptions.empty() &&
                            (prefixOptions.empty() || eRand::randChance(0.5f));
        if(suffix) {
            const auto id = eRand::randomElement(suffixOptions);
            suffixOptions.erase(id);
            if(item.fSuffix == 0) item.fSuffix = id;
            const auto& mod = eItemAffixes::sSuffixes.get(id);
            mod.generate(level, remWorth, mods);
        } else {
            const auto id = eRand::randomElement(prefixOptions);
            prefixOptions.erase(id);
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
    if(worth < 1.f) {
        if(eRand::randChance(0.5f)) {
            const uint32_t count = 1 + 5.f*sqrt(level)*worth;
            return generateGold(count);
        } else {
            return generatePotion(level, worth);
        }
    } else {
        std::vector<int> typeIds;
        for(int i = 0; i < eItemsData::sItems.size(); i++) {
            const auto& itemData = eItemsData::get(i);
            const int levelReq = itemData.fLevelReq;
            if(levelReq > level) continue;
            switch(itemData.fType) {
            case eItemType::gold:
            case eItemType::questItem:
            case eItemType::potion:
                continue;
            default:
                break;
            }

            typeIds.emplace_back(i);
        }
        if(typeIds.empty()) return eItem();
        typeId = eRand::randomElement(typeIds);
    }
    return generateItem(typeId, level, worth);
}

eItem eItemGenerator::generateGold(
    const uint32_t count) {
    const uint32_t ngold = eItemsData::sGoldIds.size();
    if(ngold == 0) return eItem();
    eItem item;
    eItemGenerator::applyItemId(item);
    item.fType = eItemType::gold;
    item.fCount = count;
    const uint32_t id = std::min(count/500, ngold - 1);
    item.fDataId = eItemsData::sGoldIds[id];
    return item;
}

void eItemGenerator::clear() {
    sNextItemId = 1;
}

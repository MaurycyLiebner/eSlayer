#include "eSlayerHelpers/eitemaffixes.h"

#include "eSlayerHelpers/erand.h"
#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eitemsdata.h"

const int eItemAffixes::sMaxItemLevel = 99;
std::map<int, std::set<int>>
eItemAffixes::sTypePrefixes;
eStringIdMapVector<eItemAffix>
eItemAffixes::sPrefixes;
std::map<int, std::set<int>>
eItemAffixes::sTypeSuffixes;
eStringIdMapVector<eItemAffix>
eItemAffixes::sSuffixes;
bool eItemAffixes::sLoaded = false;

void eItemAffixes::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Items";

    const auto parse = [&](const std::string& affixFolder,
                           const std::string& affixName,
                           eStringIdMapVector<eItemAffix>& modMap,
                           std::map<int, std::set<int>>& typeMap) {
        const auto affixPath = affixFolder + "/" + affixName;
        std::vector<std::string> affixes;
        try {
            const auto jdata = eFileLoaderBase::parse(
                dir, "/Affixes/" + affixPath + ".json");
            affixes = jdata.get<std::vector<std::string>>();
        } catch(...) {
            eRuntimeThrow("Failed to parse " + dir +
                          "/Affixes/" + affixPath + ".json");
        }
        modMap.reserve(affixes.size());
        for(const auto& name : affixes) {
            try {
                eItemAffix mod;
                const auto jdata = eFileLoaderBase::parse(
                    dir, "Affixes/" + affixFolder + "/" + name + ".json");
                if(jdata.contains("itemTypes")) {
                    const auto types = jdata.value(
                        "itemTypes", std::vector<std::string>());
                    for(const auto& typeStr : types) {
                        const auto type = eItemTypeHelpers::type(typeStr);
                        if(type == eItemType::none) {
                            eRuntimeThrow("Unrecognised item type \"" + typeStr + "\"");
                        }
                        for(const auto& it : eItemsData::sItems) {
                            const auto& item = it.fValue;
                            if(item.fType != type) continue;
                            const auto id = it.fId;
                            typeMap[id].emplace(modMap.size());
                        }
                    }
                }
                if(jdata.contains("items")) {
                    const auto items = jdata.value(
                        "items", std::vector<std::string>());
                    for(const auto& itemStr : items) {
                        const auto itemId = eItemsData::id(itemStr);
                        if(itemId < 0) {
                            eRuntimeThrow("Unrecognised item type \"" + itemStr + "\"");
                        }
                        typeMap[itemId].emplace(modMap.size());
                    }
                }
                if(jdata.contains("modifiers")) {
                    const auto& jmods = jdata["modifiers"];
                    eModsCollectionLevel::parseLevels(
                        jmods, mod.fLevels, sMaxItemLevel, true);
                    for(auto& level : mod.fLevels) {
                        level.fModifiers.collapseSkillLevel();
                        level.fTotalModifiers.collapseSkillLevel();
                    }
                }
                mod.fColor = eColor{1.f, 1.f, 1.f, 1.f};
                if(jdata.contains("color")) {
                    const auto& color = jdata["color"];
                    mod.fColor.fR = color.value("r", 1.f);
                    mod.fColor.fG = color.value("g", 1.f);
                    mod.fColor.fB = color.value("b", 1.f);
                    mod.fColor.fA = color.value("a", 1.f);
                }
                mod.fLevelReq = jdata.value("levelReq", 1);
                modMap.add(name, mod);
            } catch(...) {
                eRuntimeThrow("Failed to parse affix \"" + name + "\".");
            }
        }
    };

    sPrefixes.add("none", eItemAffix());
    parse("Prefixes", "prefixes", sPrefixes, sTypePrefixes);
    sSuffixes.add("none", eItemAffix());
    parse("Suffixes", "suffixes", sSuffixes, sTypeSuffixes);
}

void eItemAffix::generate(const int level, float& remWorth,
                          std::vector<eModifier>& mods) const {
    const float maxWorth = std::clamp(remWorth, 0.5f, 1.f);
    const float worth = eRand::randF(0.5f, maxWorth);
    remWorth -= worth;
    const auto stats = fLevels.skillLevel(level);
    for(const auto& it : stats.fTotalModifiers) {
        auto mod = it.second;
        mod.fValue1 *= worth;
        mod.fValue2 *= worth;
        mod.fValue1 = eModifierHelpers::clampValue(
            mod.fValue1, mod.fType);
        mod.fValue2 = eModifierHelpers::clampValue(
            mod.fValue2, mod.fType);
        mods.push_back(mod);
    }
}

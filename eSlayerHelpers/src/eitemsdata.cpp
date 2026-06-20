#include "eSlayerHelpers/eitemsdata.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eexceptions.h"
#include "eSlayerHelpers/eweapontype.h"
#include "eSlayerHelpers/epotiontype.h"

// #include <fstream>

std::vector<int> eItemsData::sGoldIds;
bool eItemsData::sLoaded = false;
eStringIdMapVector<eItemData>
eItemsData::sItems;

eItemData& eItemsData::get(const std::string& name) {
    return get(id(name));
}

eItemData& eItemsData::get(const int id) {
    if(id < 0 || id >= sItems.size()) {
        eRuntimeThrow("Index out of range.");
    }
    return sItems.get(id);
}

int eItemsData::id(const std::string& name) {
    return sItems.id(name);
}

std::string eItemsData::name(const int id) {
    return sItems.name(id);
}

void eItemsData::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Items";

    ePotionTypes::sTypes.add("", false);

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "items.json");
        for(auto it = jdata.begin(); it != jdata.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();
            const auto itemType = eItemTypeHelpers::type(key);
            if(key == "weapon") {
                for(auto& [type, types] : value.items()) {
                    for(const auto& name : types) {
                        load(name, itemType);
                    }
                }
            } else {
                for(const auto& name : value) {
                    load(name, itemType);
                }
            }
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/items.json");
    }
}

void eItemsData::load(const std::string& name,
                      const eItemType type) {
    const auto dir = "Items";

    // {
    //     const auto filePath = eFileLoaderBase::sFilePath(dir, name + ".json");
    //     std::ofstream output(filePath);
    //     output << "{" << std::endl << std::endl << "}";
    // }

    const auto jdata = eFileLoaderBase::parse(dir, name + ".json");

    eItemData itemData;
    itemData.fType = type;
    if(type == eItemType::weapon) {
        const std::string subtypeStr = jdata.value("subtype", "meele");
        const auto weaponType = eWeaponTypeHelpers::type(subtypeStr);
        itemData.fSubtype = static_cast<uint8_t>(weaponType);

        itemData.fTwoHanded = jdata.value("twoHanded", false);

        const auto secondHand = jdata.value(
            "secondHand", std::vector<std::string>());
        itemData.fSecondHand.emplace_back(eItemType::none);
        for(const auto& typeStr : secondHand) {
            const auto type = eItemTypeHelpers::type(typeStr);
            itemData.fSecondHand.emplace_back(type);
        }

        itemData.fMissileStr = jdata.value("missile", "");
        itemData.fMissileRadius = jdata.value("missileRadius", 0.1f);
        itemData.fMissileSpeed = jdata.value("missileSpeed", 0.25f);
    } else if(type == eItemType::potion) {
        const auto typeStr = jdata.value("type", "");
        if(typeStr.empty()) {
            eRuntimeThrow("No potion type provided for \"" + name + "\"");
        }
        int typeId = ePotionTypes::sTypes.id(typeStr);
        if(typeId < 0) {
            typeId = ePotionTypes::sTypes.add(typeStr, true);
        }
        itemData.fSubtype = typeId;

        itemData.fPotionFrameLength = jdata.value("frameLength", 0.f);

        itemData.fPotionInstantHealth = jdata.value("instantHealth", 0.f);
        itemData.fPotionInstantMana = jdata.value("instantMana", 0.f);
        itemData.fPotionInstantStamina = jdata.value("instantStamina", 0.f);

        itemData.fPotionInstantHealthFrac = jdata.value("instantHealthFrac", 0.f);
        itemData.fPotionInstantManaFrac = jdata.value("instantManaFrac", 0.f);
        itemData.fPotionInstantStaminaFrac = jdata.value("instantStaminaFrac", 0.f);

        itemData.fPotionTotalHealth = jdata.value("totalHealth", 0.f);
        itemData.fPotionTotalMana = jdata.value("totalMana", 0.f);
        itemData.fPotionTotalStamina = jdata.value("totalStamina", 0.f);

        itemData.fPotionTotalHealthFrac = jdata.value("totalHealthFrac", 0.f);
        itemData.fPotionTotalManaFrac = jdata.value("totalManaFrac", 0.f);
        itemData.fPotionTotalStaminaFrac = jdata.value("totalStaminaFrac", 0.f);

        if(jdata.contains("modifiers")) {
            const auto boostCurseTypeStr = jdata.value("boostCurseType", "");
            const int id = eBoostCurseTypes::sTypes.id(boostCurseTypeStr);
            if(id <= 0) {
                eRuntimeThrow("Invalid \"boostCurseType\" \"" + boostCurseTypeStr +
                              "\" in \"" + dir + "/" + name + ".json\"");
            }
            itemData.fPotionBoostType = static_cast<eBoostCurseType>(id);
            const auto& mods = jdata["modifiers"];
            for(const auto& [name, modData] : mods.items()) {
                auto& mod = itemData.fPotionMods.emplace_back();
                mod.read(name, json(modData));
            }
        }
    } else if(type == eItemType::gold) {
        const int id = sItems.nextId();
        sGoldIds.emplace_back(id);
    }
    itemData.fRange = jdata.value("range", 0.f);
    itemData.fWSM = jdata.value("WSM", 0.f);
    itemData.fLevelReq = jdata.value("levelReq", 1);
    itemData.fStrengthReq = jdata.value("strReq", 0);
    itemData.fDexterityReq = jdata.value("dexReq", 0);
    sItems.add(name, itemData);
}

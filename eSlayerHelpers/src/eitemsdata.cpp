#include "eSlayerHelpers/eitemsdata.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eexceptions.h"
#include "eSlayerHelpers/eweapontype.h"

// #include <fstream>

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
        for(const auto& typeStr : secondHand) {
            const auto type = eItemTypeHelpers::type(typeStr);
            itemData.fSecondHand.emplace_back(type);
        }

        itemData.fMissileStr = jdata.value("missile", "");
        itemData.fMissileRadius = jdata.value("missileRadius", 0.1f);
        itemData.fMissileSpeed = jdata.value("missileSpeed", 0.25f);
    }
    itemData.fRange = jdata.value("range", 0.f);
    itemData.fLevelReq = jdata.value("levelReq", 1);
    itemData.fStrengthReq = jdata.value("strReq", 0);
    itemData.fDexterityReq = jdata.value("dexReq", 0);
    sItems.add(name, itemData);
}

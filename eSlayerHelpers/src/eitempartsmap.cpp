#include "eSlayerHelpers/eitempartsmap.h"

#include "eSlayerHelpers/eexceptions.h"
#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eitemsdata.h"

std::map<eItemPlace, eItemStrMap>
eItemPartsMap::sMap;
eStrMap eItemPartsMap::sPartsBase;
bool eItemPartsMap::sLoaded = false;

void eItemPartsMap::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Items";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "itemPartsBase.json");
        for(auto it = jdata.begin(); it != jdata.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();
            sPartsBase[key] = value;
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/itemPartsBase.json");
    }

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "itemPartsMap.json");
        for(auto it = jdata.begin(); it != jdata.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();

            eItemPlace itemPlace;
            if(key == "weaponR") {
                itemPlace = eItemPlace::weaponR;
            } else if(key == "weaponL") {
                itemPlace = eItemPlace::weaponL;
            } else if(key == "helmet") {
                itemPlace = eItemPlace::helmet;
            } else if(key == "armor") {
                itemPlace = eItemPlace::armor;
            } else {
                eRuntimeThrow("Unrecognized item place \"" + key + "\".");
            }

            auto& items = sMap[itemPlace];
            switch(itemPlace) {
            case eItemPlace::weaponR:
            case eItemPlace::weaponL: {
                for(auto it = value.begin(); it != value.end(); ++it) {
                    const auto& key = it.key();
                    const auto& value = it.value();
                    std::vector<int> itemIds;
                    if(key == "") {
                        itemIds.emplace_back(-1);
                    } else {
                        for(const auto& it : eItemsData::sItems) {
                            const auto& value = it.fValue;
                            const auto& name = value.fTextureStr;
                            if(name != key) continue;
                            const int itemId = it.fId;
                            itemIds.emplace_back(itemId);
                        }
                    }
                    for(const int itemId : itemIds) {
                        auto& item = items[itemId];
                        for(auto it = value.begin(); it != value.end(); ++it) {
                            const auto& key = it.key();
                            const auto& value = it.value();
                            item[key] = value;
                        }
                    }
                }
            } break;
            case eItemPlace::helmet:
            case eItemPlace::armor: {
                const std::string key = itemPlace == eItemPlace::helmet ?
                    "helmet" : "armor";
                const std::vector<std::string> types = value;
                for(const auto& type : types) {
                    std::vector<int> itemIds;
                    if(key == "") {
                        itemIds.emplace_back(-1);
                    } else {
                        for(const auto& it : eItemsData::sItems) {
                            const auto& value = it.fValue;
                            const auto& name = value.fTextureStr;
                            if(name != type) continue;
                            const int itemId = it.fId;
                            itemIds.emplace_back(itemId);
                        }
                    }
                    for(const int itemId : itemIds) {
                        auto& item = items[itemId];
                        item[key] = type;
                    }
                }
            } break;
            };
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/itemPartsMap.json");
    }
}

const eStrMap& eItemPartsMap::get(
    const eItemPlaceItem item) {
    return sMap[item.fPlace][item.fItemId];
}

eStrMap eItemPartsMap::get(
    const std::vector<eItemPlaceItem> items) {
    eStrMap result;
    for(const auto& item : items) {
        const auto& map = get(item);
        for(const auto& it : map) {
            auto& dst = result[it.first];
            const auto& src = it.second;
            if(!src.empty()) dst = src;
        }
    }

    return result;
}
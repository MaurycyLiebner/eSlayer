#include "eSlayerHelpers/eitempartsmap.h"

#include "eSlayerHelpers/eexceptions.h"
#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eitemsdata.h"

std::map<eItemPlace, eItemStrMap>
eItemPartsMap::sMap;
bool eItemPartsMap::sLoaded = false;

void eItemPartsMap::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Items";

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
            } else {
                eRuntimeThrow("Unrecognized item place \"" + key + "\".");
            }

            auto& items = sMap[itemPlace];
            for(auto it = value.begin(); it != value.end(); ++it) {
                const auto& key = it.key();
                const auto& value = it.value();
                int itemId = -1;
                if(key == "") {
                    itemId = -1;
                } else {
                    itemId = eItemsData::id(key);
                }
                auto& item = items[itemId];
                for(auto it = value.begin(); it != value.end(); ++it) {
                    const auto& key = it.key();
                    const auto& value = it.value();
                    item[key] = value;
                }
            }
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
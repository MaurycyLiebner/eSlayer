#include "eitemsounds.h"

#include "esounds.h"

#include <eSlayerHelpers/efileloaderbase.h>
#include <eSlayerHelpers/eitemsdata.h>

eStringIdMapVector<int> eItemSounds::sSoundIds;
bool eItemSounds::sLoaded = false;

void eItemSounds::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Audio";

    try {
        std::map<int, int> map;
        const auto jdata = eFileLoaderBase::parse(dir, "itemSounds.json");
        for(auto it = jdata.begin(); it != jdata.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();
            const int itemId = eItemsData::sItems.id(key);
            if(itemId < 0) {
                eRuntimeThrow("Unrecognized item \"" + key + "\".");
            }
            std::string name;
            if(value.is_boolean()) {
                const bool r = value;
                if(!r) continue;
                name = key;
            } else {
                name = value;
            }
            const int soundId = eSounds::sSounds.id(name);
            map[itemId] = soundId;
        }

        for(const auto& it : eItemsData::sItems) {
            int soundId = -1;
            const auto sit = map.find(it.fId);
            if(sit != map.end()) {
                soundId = sit->second;
            }
            sSoundIds.add(it.fName, soundId);
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/itemSounds.json");
    }
}

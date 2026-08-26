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
        std::map<std::string, int> map;
        const auto jdata = eFileLoaderBase::parse(dir, "itemSounds.json");
        for(auto it = jdata.begin(); it != jdata.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();
            std::string name;
            if(value.is_boolean()) {
                const bool r = value;
                if(!r) continue;
                name = key;
            } else {
                name = value;
            }
            const int soundId = eSounds::sSounds.id(name);
            map[key] = soundId;
        }

        for(const auto& it : eItemsData::sItems) {
            int soundId = -1;
            const auto& value = it.fValue;
            const auto sit = map.find(value.fTextureStr);
            if(sit != map.end()) {
                soundId = sit->second;
            }
            sSoundIds.add(it.fName, soundId);
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/itemSounds.json");
    }
}

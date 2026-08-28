#include "eitemsounds.h"

#include "esounds.h"

#include <eSlayerHelpers/efileloaderbase.h>
#include <eSlayerHelpers/eitemsdata.h>

eStringIdMapVector<eItemSound> eItemSounds::sSoundIds;
bool eItemSounds::sLoaded = false;

void eItemSounds::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Audio";

    try {
        std::map<std::string, eItemSound> map;
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
            eItemSound sound;
            sound.fHit = eSounds::sSounds.id(name + "Hit");
            sound.fMiss = eSounds::sSounds.id(name + "Miss");
            if(sound.fMiss == -1) {
                sound.fMiss = eSounds::sSounds.id("miss");
            }
            map[key] = sound;
        }

        for(const auto& it : eItemsData::sItems) {
            eItemSound sound;
            const auto& value = it.fValue;
            const auto sit = map.find(value.fTextureStr);
            if(sit != map.end()) {
                sound = sit->second;
            }
            sSoundIds.add(it.fName, sound);
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/itemSounds.json");
    }
}

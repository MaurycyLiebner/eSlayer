#include "eSlayerHelpers/esoundsbase.h"

#include "eSlayerHelpers/efileloaderbase.h"

eStringIdMapVector<std::vector<std::string>>
eSoundsBase::sSounds;
bool eSoundsBase::sLoaded = false;

void eSoundsBase::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Audio";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "Wavs/wavs.json");
        for(auto& [key, value] : jdata.items()) {
            std::vector<std::string> values;
            if(value.is_boolean()) {
                const bool r = value;
                if(r) {
                    values.emplace_back(key);
                } else {
                    continue;
                }
            } else if(value.is_string()) {
                values.emplace_back(value);
            } else {
                values = value;
            }
            sSounds.add(key, values);
       }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/Wavs/wavs.json");
    }
}
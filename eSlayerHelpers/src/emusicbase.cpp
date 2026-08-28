#include "eSlayerHelpers/emusicbase.h"

#include "eSlayerHelpers/efileloaderbase.h"

eStringIdMapVector<std::vector<std::string>>
eMusicBase::sMusic;
bool eMusicBase::sLoaded = false;

void eMusicBase::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Audio";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "Music/music.json");
        for(auto& [key, value] : jdata.items()) {
            std::vector<std::string> values;
            if(value.is_boolean()) {
                const bool r = value;
                if(r) {
                    values.emplace_back(key);
                } else {
                    continue;
                }
            } else if(value.is_number_integer()) {
                const int n = value;
                for(int i = 1; i <= n; i++) {
                    values.emplace_back(key + std::to_string(i));
                }
            } else if(value.is_string()) {
                values.emplace_back(value);
            } else {
                values = value;
            }
            sMusic.add(key, values);
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "Music/music.json");
    }
}
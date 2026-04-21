#include "emapsettings.h"

#include <eSlayerHelpers/efileloaderbase.h>
#include <eSlayerHelpers/echardatainfo.h>

std::map<std::string, eMapSettings>
eMapSettings::sMaps;
bool eMapSettings::sLoaded = false;

void eMapSettings::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Maps";
    std::vector<std::string> maps;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, "maps.json");
        maps = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/maps.json");
    }

    for(const auto& name : maps) {
        try {
            const auto jdata = eFileLoaderBase::parse(dir, name + ".json");
            eMapSettings map;
            const auto monsters = jdata.value("monsters", std::vector<std::string>());
            for(const auto& m : monsters) {
                const auto unitId = eCharDataInfo::id(m);
                map.fMonsters.emplace(unitId);
            }
            map.fLightness = jdata.value("lightness", 180);
            map.fContrast = jdata.value("contrast", 140);
            sMaps[name] = map;
        } catch(...) {
            eRuntimeThrow("Failed to parse " + dir + "/" + name + ".json");
        }
    }
}

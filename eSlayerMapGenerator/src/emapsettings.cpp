#include "emapsettings.h"

#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/efileloaderbase.h>

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

            const auto& jAreas = jdata.at("areas");

            for(auto it = jAreas.begin(); it != jAreas.end(); ++it) {
                const std::string areaName = it.key();
                const auto& jArea = it.value();

                eAreaSettings area;
                const auto areaTypeStr = jArea.value("type", "open");
                if(areaTypeStr == "dungeon") {
                    area.fType = eAreaType::dungeon;
                } else {
                    area.fType = eAreaType::open;
                }
                const auto terrTypeStr = jArea.value("terrain", "grass");
                area.fTerrainType = eTerrsTexturesData::id(terrTypeStr);
                // lightness / contrast
                area.fLightness = jArea.value("lightness", 180);
                area.fContrast  = jArea.value("contrast", 140);

                // monsters
                const auto monsters = jArea.value("monsters", std::vector<std::string>());
                for(const auto& m : monsters) {
                    const auto unitId = eCharDataInfo::id(m);
                    area.fMonsters.emplace(unitId);
                }

                // connections
                if(jArea.contains("connections")) {
                    const auto& items = jArea["connections"];
                    for(auto cit = items.begin(); cit != items.end(); ++cit) {
                        const auto target = cit.key();
                        const auto& jConn = cit.value();

                        eConnectionType conn;
                        const auto type = jConn.value("type", "plain");
                        if(type == "cave") {
                            conn = eConnectionType::cave;
                        } else if(type == "portal") {
                            conn = eConnectionType::portal;
                        } else {
                            conn = eConnectionType::plain;
                        }
                        area.fConnections[target] = conn;
                    }
                }
                map.fAreas.add(areaName, area);
            }

            sMaps[name] = map;
        } catch(...) {
            eRuntimeThrow("Failed to parse " + dir + "/" + name + ".json");
        }
    }
}

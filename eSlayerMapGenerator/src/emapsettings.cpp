#include "eSlayerMapGenerator/emapsettings.h"

#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/efileloaderbase.h>
#include <eSlayerHelpers/eobjectsinfo.h>

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
                if(jArea.contains("monsters")) {
                    auto& monsters = area.fMonsters;
                    auto& mtypes = monsters.fTypes;
                    monsters.fMonstersMargin = jArea.value("monsterMargin", 4);
                    monsters.fRectMargin = jArea.value("wallMargin", 4);
                    const auto& items = jArea["monsters"];
                    for(auto cit = items.begin(); cit != items.end(); ++cit) {
                        const auto mname = cit.key();
                        const auto& values = cit.value();
                        eMonsterProbability result;
                        result.fProbability = values.value("probability", 0.f);
                        result.fGroupSize = values.value("groupSize", 1);
                        result.fEliteProbability = values.value("eliteProbability", 0.f);
                        const auto type = eCharDataInfo::id(mname);
                        result.fType = type;
                        if(type == -1) {
                            eRuntimeThrow("Invalid monster type \"" + name +
                                          "\" in " + dir + "/" + name + ".json");
                        }
                        mtypes.emplace_back(result);
                    }
                }

                area.fObjectsMargin = jArea.value("objectsMargin", 4);
                // objects
                if(jArea.contains("objects")) {
                    const auto& items = jArea["objects"];
                    for(auto cit = items.begin(); cit != items.end(); ++cit) {
                        const auto oname = cit.key();
                        const float prob = cit.value();
                        const int type = eObjectsInfo::sObjects.id(oname);
                        if(type == -1) {
                            eRuntimeThrow("Invalid object type \"" + name +
                                          "\" in " + dir + "/" + name + ".json");
                        }
                        area.fObjects.emplace_back(type, prob);
                    }
                }

                area.fOutsideObjectsMargin = jArea.value("outObjectsMargin", 1);
                // out objects
                if(jArea.contains("outObjects")) {
                    const auto& items = jArea["outObjects"];
                    for(auto cit = items.begin(); cit != items.end(); ++cit) {
                        const auto oname = cit.key();
                        const float prob = cit.value();
                        const int type = eObjectsInfo::sObjects.id(oname);
                        if(type == -1) {
                            eRuntimeThrow("Invalid object type \"" + name +
                                          "\" in " + dir + "/" + name + ".json");
                        }
                        area.fOutsideObjects.emplace_back(type, prob);
                    }
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

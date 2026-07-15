#include "eSlayerHelpers/emapsettings.h"

#include "eSlayerHelpers/eterrstexturesdata.h"
#include "eSlayerHelpers/echardatainfo.h"
#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eobjectsinfo.h"
#include "eSlayerHelpers/eblueprints.h"
#include "eSlayerHelpers/ewaypoints.h"

eStringIdMapVector<eMapSettings>
eMapsSettings::sMaps;
bool eMapsSettings::sLoaded = false;

void eMapsSettings::load() {
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

            map.fMaxSize = jdata.value("maxSize", 80);
            const auto respawnMapName = jdata.value("respawnMap", name);
            const int respawnMapId = respawnMapName == name ?
                sMaps.size() : sMaps.id(respawnMapName);
            if(respawnMapId < 0) {
                eRuntimeThrow("Not recognized \"respawnMap\" \"" +
                              respawnMapName + "\".");
            }
            map.fRespawnMap = respawnMapId;
            map.fActId = jdata.value("act", 0);

            const auto& jAreas = jdata.at("areas");

            for(auto it = jAreas.begin(); it != jAreas.end(); ++it) {
                const std::string areaName = it.key();
                const auto& jArea = it.value();

                eAreaSettings area;
                area.fName = areaName;
                const auto areaTypeStr = jArea.value("type", "open");
                if(areaTypeStr == "dungeon") {
                    area.fType = eAreaType::dungeon;
                } else if(areaTypeStr == "open") {
                    area.fType = eAreaType::open;
                } else if(areaTypeStr == "camp") {
                    area.fType = eAreaType::camp;
                } else {
                    eRuntimeThrow("Invalid area type \"" + areaTypeStr + "\".");
                }
                const auto terrTypeStr = jArea.value("terrain", "grass");
                area.fTerrainType = eTerrsTexturesData::id(terrTypeStr);
                area.fWaypoint = jArea.value("waypoint", false);
                area.fLightness = jArea.value("lightness", 180);
                area.fContrast = jArea.value("contrast", 140);
                area.fLevel = jArea.value("level", 0);
                area.fSize = jArea.value("size", 50);

                if(jArea.contains("monsters")) {
                    auto& monsters = area.fMonsters;
                    auto& mtypes = monsters.fTypes;
                    for(const auto& values : jArea["monsters"]) {
                        eMonsterCount result;
                        const auto mname = values.value("type", "");
                        result.fBaseType = eCharDataInfo::id(mname);
                        if(result.fBaseType < 0) {
                            eRuntimeThrow("Invalid monster type \"" + mname +
                                          "\" in " + dir + "/" + name + ".json");
                        }
                        result.fCount = values.value("count", 1);
                        result.fGroupSize = values.value("groupSize", 1);
                        result.fElite = values.value("elite", false);
                        result.fMinArea = values.value("minArea", 0);
                        const auto typeNames = values.value("types", std::vector<std::string>({mname}));
                        for(const auto& typeName : typeNames) {
                            const auto type = eCharDataInfo::id(typeName);
                            if(type < 0) {
                                eRuntimeThrow("Invalid monster type \"" + typeName +
                                              "\" in " + dir + "/" + name + ".json");
                            }
                            result.fTypes.emplace_back(type);
                        }
                        const auto bossTypeNames = values.value("bossTypes", typeNames);
                        for(const auto& typeName : bossTypeNames) {
                            const auto type = eCharDataInfo::id(typeName);
                            if(type < 0) {
                                eRuntimeThrow("Invalid monster type \"" + typeName +
                                              "\" in " + dir + "/" + name + ".json");
                            }
                            result.fBossTypes.emplace_back(type);
                        }
                        mtypes.emplace_back(result);
                    }
                }

                const auto parseObjects = [&](const ordered_json& items,
                                              std::vector<eObjectCount>& vec) {
                    for(const auto& values : items) {
                        const auto oname = values.value("type", "");
                        const int type = eObjectsInfo::sObjects.id(oname);
                        if(type == -1) {
                            eRuntimeThrow("Invalid object type \"" + oname +
                                          "\" in " + dir + "/" + name + ".json");
                        }
                        const int count = values.value("count", 1);
                        const int minArea = values.value("minArea", 0);
                        std::optional<uint8_t> subtype;
                        if(values.contains("subtype")) {
                            subtype = values.value("subtype", 0);
                        }
                        vec.emplace_back(type, subtype, count, minArea);
                    }
                };

                if(jArea.contains("objects")) {
                    const auto& items = jArea["objects"];
                    parseObjects(items, area.fObjects);
                }

                if(jArea.contains("outObjects")) {
                    const auto& items = jArea["outObjects"];
                    parseObjects(items, area.fOutsideObjects);
                }

                const auto parseBlueprints = [&](const ordered_json& items,
                                                 std::vector<eBlueprintCount>& vec) {
                    for(const auto& values : items) {
                        const auto bpname = values.value("type", "");
                        const int type = eBlueprints::sBlueprints.id(bpname);
                        if(type == -1) {
                            eRuntimeThrow("Invalid blueprint type \"" + bpname +
                                          "\" in " + dir + "/" + name + ".json");
                        }
                        const int count = values.value("count", 1);
                        vec.emplace_back(type, count);
                    }
                };

                if(jArea.contains("blueprints")) {
                    const auto& items = jArea["blueprints"];
                    parseBlueprints(items, area.fBlueprints);
                }

                if(jArea.contains("effects")) {
                    const auto& effects = jArea["effects"];
                    for(const auto& effect : effects) {
                        auto& e = area.fEffects.emplace_back();
                        const auto typeStr = effect.value("type", "");
                        if(typeStr == "distort") {
                            e.fType = eEffectType::distort;
                        } else {
                            eRuntimeThrow("Unrecognized effect type \"" + typeStr + "\".");
                        }
                        e.fScale = effect.value("scale", 1.f);
                        e.fSpeed = effect.value("speed", 1.f);
                    }
                }

                if(jArea.contains("connections")) {
                    const auto& items = jArea["connections"];
                    for(auto cit = items.begin(); cit != items.end(); ++cit) {
                        const auto target = cit.key();
                        const auto& jConn = cit.value();

                        eConnectionType connType;
                        const auto type = jConn.value("type", "plain");
                        if(type == "cave") {
                            connType = eConnectionType::cave;
                        } else if(type == "portal") {
                            connType = eConnectionType::portal;
                        } else if(type == "stairs") {
                            connType = eConnectionType::stairs;
                        } else if(type == "plain") {
                            connType = eConnectionType::plain;
                        } else if(type == "trapDoor") {
                            connType = eConnectionType::trapDoor;
                        } else {
                            eRuntimeThrow("Invalid connection type \"" + type + "\".");
                        }
                        auto& conn = area.fConnections[target];
                        conn.fType = connType;
                        const bool entrance = jConn.value("entrance", false);
                        if(entrance) {
                            conn.fDir = eConnectionDir::down;
                        } else {
                            conn.fDir = eConnectionDir::up;
                        }
                        conn.fMap = jConn.value("map", "");
                    }
                }
                const auto areaId = map.fAreas.add(areaName, area);

                if(area.fWaypoint) {
                    auto& w = eWaypoint::sWaypoints.emplace_back();
                    w.fActId = map.fActId;
                    w.fArea = eAreaIds(sMaps.nextId(), areaId);
                    w.fKnown = false;
                }
            }

            sMaps.add(name, map);
        } catch(...) {
            eRuntimeThrow("Failed to parse " + dir + "/" + name + ".json");
        }
    }
}

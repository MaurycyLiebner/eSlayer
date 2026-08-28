#include "eSlayerHelpers/emapsettings.h"

#include "eSlayerHelpers/eterrstexturesdata.h"
#include "eSlayerHelpers/eunitsinfo.h"
#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eobjectsinfo.h"
#include "eSlayerHelpers/eblueprints.h"
#include "eSlayerHelpers/ewaypoints.h"
#include "eSlayerHelpers/edifficulties.h"
#include "eSlayerHelpers/emusicbase.h"

eStringIdMapVector<eAreaSettings>
eMapsSettings::sAreas;
eStringIdMapVector<eMapSettings>
eMapsSettings::sMaps;
bool eMapsSettings::sLoaded = false;

void parseArea(eAreaSettings& area, const ordered_json& jArea) {
    const auto musicStr = jArea.value("music", "");
    if(musicStr.empty()) {
        area.fMusic = -1;
    } else {
        const int id = eMusicBase::sMusic.id(musicStr);
        if(id < 0) {
            eRuntimeThrow("Unrecognized music \"" + musicStr + "\".");
        }
        area.fMusic = id;
    }

    area.fLightness = jArea.value("lightness", area.fLightness);
    area.fContrast = jArea.value("contrast", area.fContrast);
    area.fSize = jArea.value("size", area.fSize);

    area.fRoomSize = jArea.value("roomSize", area.fRoomSize);
    area.fConnThick = jArea.value("connThick", area.fConnThick);
    area.fConnLen = jArea.value("connLen", area.fConnLen);

    area.fMerge = jArea.value("merge", area.fMerge);

    const auto parse = [](const ordered_json& jdata,
                          eAreaTemplate& area) {
        area.fLevel = jdata.value("level", 1);

        const auto parseMonsters = [](const ordered_json& items,
                                      std::vector<eMonsterCount>& vec) {
            for(const auto& values : items) {
                eMonsterCount result;
                const auto mname = values.value("type", "");
                result.fBaseType = eUnitsInfo::sUnits.id(mname);
                if(result.fBaseType < 0) {
                    eRuntimeThrow("Invalid monster type \"" + mname + "\".");
                }
                result.fCount = values.value("count", 1);
                result.fGroupSize = values.value("groupSize", 1);
                result.fElite = values.value("elite", false);
                result.fMinArea = values.value("minArea", 0);
                const auto typeNames = values.value("types", std::vector<std::string>({mname}));
                for(const auto& typeName : typeNames) {
                    const auto type = eUnitsInfo::sUnits.id(typeName);
                    if(type < 0) {
                        eRuntimeThrow("Invalid monster type \"" + typeName + "\".");
                    }
                    result.fTypes.emplace_back(type);
                }
                const auto bossTypeNames = values.value("bossTypes", typeNames);
                for(const auto& typeName : bossTypeNames) {
                    const auto type = eUnitsInfo::sUnits.id(typeName);
                    if(type < 0) {
                        eRuntimeThrow("Invalid monster type \"" + typeName + "\".");
                    }
                    result.fBossTypes.emplace_back(type);
                }

                if(values.contains("items")) {
                    for(const auto& jitem : values["items"]) {
                        auto& item = result.fItemDrops.emplace_back();
                        item.read(jitem);
                    }
                }

                vec.emplace_back(result);
            }
        };

        if(jdata.contains("monsters")) {
            const auto& monsters = jdata["monsters"];
            parseMonsters(monsters, area.fMonsters);
        }

        const auto parseObjects = [](const ordered_json& items,
                                     std::vector<eObjectCount>& vec) {
            for(const auto& values : items) {
                const auto oname = values.value("type", "");
                const int type = eObjectsInfo::sObjects.id(oname);
                if(type == -1) {
                    eRuntimeThrow("Invalid object type \"" + oname + ".\"");
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

        if(jdata.contains("objects")) {
            const auto& items = jdata["objects"];
            parseObjects(items, area.fObjects);
        }

        if(jdata.contains("outObjects")) {
            const auto& items = jdata["outObjects"];
            parseObjects(items, area.fOutsideObjects);
        }

        const auto parseBlueprints = [](const ordered_json& items,
                                        std::vector<eBlueprintCount>& vec) {
            for(const auto& values : items) {
                const auto bpname = values.value("type", "");
                const int type = eBlueprints::sBlueprints.id(bpname);
                if(type == -1) {
                    eRuntimeThrow("Invalid blueprint type \"" + bpname + "\".");
                }
                const int count = values.value("count", 1);
                vec.emplace_back(type, count);
            }
        };

        if(jdata.contains("blueprints")) {
            const auto& items = jdata["blueprints"];
            parseBlueprints(items, area.fBlueprints);
        }
    };

    const auto parseTemplate = [&](const ordered_json& jdata,
                                   eAreaTemplate& area) {
        if(jdata.is_string()) {
            const auto dir = "Maps";
            const std::string name = jdata;
            const auto jdata = eFileLoaderBase::parse(
                dir, "templates/" + name + ".json");
            parse(jdata, area);
        } else {
            parse(jdata, area);
        }
    };

    if(jArea.contains("base")) {
        const auto& base = jArea["base"];
        parseTemplate(base, area.fBase);
    }

    if(jArea.contains("difficulties")) {
        const auto& diffs = jArea["difficulties"];
        for(const auto& [diffStr, data] : diffs.items()) {
            const int diffId = eDifficulties::sDifficulties.id(diffStr);
            if(diffId < 0) {
                eRuntimeThrow("Unrecognized difficulty \"" + diffStr + "\".");
            }
            auto& settings = area.fDifficulties[diffId];
            settings = area.fBase;
            parseTemplate(data, settings);
        }
    }

    if(jArea.contains("effects")) {
        const auto& effects = jArea["effects"];
        for(const auto& effect : effects) {
            auto& e = area.fEffects.emplace_back();
            const auto typeStr = effect.value("type", "");
            if(typeStr == "distort") {
                e.fType = eEffectType::distort;
            } else if(typeStr == "rain") {
                e.fType = eEffectType::rain;
            } else if(typeStr == "space") {
                e.fType = eEffectType::space;
            } else {
                eRuntimeThrow("Unrecognized effect type \"" + typeStr + "\".");
            }
            e.fScale = effect.value("scale", 1.f);
            e.fSpeed = effect.value("speed", 1.f);
        }
    }
}

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
                const auto terrTypeStr = jArea.value("terrain", "");
                const auto terrId = eTerrsTexturesData::id(terrTypeStr);
                if(terrId < 0) {
                    eRuntimeThrow("Unrecognized terrain type \"" + terrTypeStr + "\".");
                }
                area.fTerrainType = terrId;

                area.fWaypoint = jArea.value("waypoint", false);

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

                parseArea(area, jArea);

                const auto areaId = eMapsSettings::sAreas.add(areaName, area);
                map.fAreas.emplace_back(areaId);

                if(area.fWaypoint) {
                    auto& w = eWaypoints::sWaypoints.emplace_back();
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

const eAreaTemplate& eAreaSettings::template_(const int diff) const {
    const auto it = fDifficulties.find(diff);
    if(it == fDifficulties.end()) return fBase;
    return it->second;
}

#include "eSlayerHelpers/eblueprints.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eobjectsinfo.h"
#include "eSlayerHelpers/eterrstexturesdata.h"

eStringIdMapVector<eBlueprint>
eBlueprints::sBlueprints;
bool eBlueprints::sLoaded = false;

void eBlueprints::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Blueprints";

    const auto path = "blueprints.json";
    std::vector<std::string> names;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, path);
        names = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/" + path);
    }

    for(const auto& name : names) {
        const auto path = name + ".json";
        try {
            eBlueprint bp;
            const auto jdata = eFileLoaderBase::parse(dir, path);
            bp.fWidth = jdata.value("width", 0.f);
            bp.fHeight = jdata.value("height", 0.f);
            if(jdata.contains("objects")) {
                for(const auto& objData : jdata["objects"]) {
                    const auto oname = objData.value("type", "");
                    const auto id = eObjectsInfo::sObjects.id(oname);
                    if(id < 0) {
                        eRuntimeThrow("Invalid object name \"" + oname +
                                      "\" in blueprint \"" + name + "\".");
                    }
                    auto& obj = bp.fObjects.emplace_back();
                    obj.fType = id;
                    obj.fSubtype = objData.value("subtype", 0);
                    obj.fX = objData.value("x", 0.f);
                    obj.fY = objData.value("y", 0.f);
                }
            }
            if(jdata.contains("terrain")) {
                for(const auto& terrData : jdata["terrain"]) {
                    const auto tname = terrData.value("type", "");
                    const auto id = eTerrsTexturesData::id(tname);
                    if(id < 0) {
                        eRuntimeThrow("Invalid terrain name \"" + tname +
                                      "\" in blueprint \"" + name + "\".");
                    }
                    auto& terr = bp.fTerrain.emplace_back();
                    terr.fType = id;
                    terr.fX = terrData.value("x", 0.f);
                    terr.fY = terrData.value("y", 0.f);
                }
            }
            sBlueprints.add(name, bp);
        } catch(...) {
            eRuntimeThrow("Failed to parse " + dir + "/" + path);
        }
    }
}
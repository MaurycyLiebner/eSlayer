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
                    if(objData.contains("subtype")) {
                        obj.fSubtype = objData.value("subtype", 0);
                    }
                    obj.fX = objData.value("x", 0.f);
                    obj.fY = objData.value("y", 0.f);
                    if(objData.contains("width")) {
                        obj.fWidth = objData.value("width", 1.f);
                    }
                    if(objData.contains("height")) {
                        obj.fHeight = objData.value("height", 1.f);
                    }
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
            if(jdata.contains("blueprints")) {
                for(const auto& jbp : jdata["blueprints"]) {
                    const auto bpname = jbp.value("type", "");
                    const auto id = eBlueprints::sBlueprints.id(bpname);
                    if(id < 0) {
                        eRuntimeThrow("Invalid blueprint name \"" + bpname +
                                      "\" in blueprint \"" + name + "\".");
                    }
                    const float dx = jbp.value("x", 0.f);
                    const float dy = jbp.value("y", 0.f);
                    auto& b = eBlueprints::sBlueprints.get(id);
                    for(const auto& o : b.fObjects) {
                        auto& obj = bp.fObjects.emplace_back(o);
                        obj.fX += dx;
                        obj.fY += dy;
                    }
                    for(const auto& t : b.fTerrain) {
                        auto& terr = bp.fTerrain.emplace_back(t);
                        terr.fX += dx;
                        terr.fY += dy;
                    }
                }
            }
            sBlueprints.add(name, bp);
        } catch(...) {
            eRuntimeThrow("Failed to parse " + dir + "/" + path);
        }
    }
}
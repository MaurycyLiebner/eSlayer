#include "eSlayerHelpers/eobjectsinfo.h"

#include "eSlayerHelpers/efileloaderbase.h"

bool eObjectsInfo::sLoaded = false;
eStringIdMapVector<eObjectInfo>
eObjectsInfo::sObjects;

void eObjectsInfo::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Objects";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "objects.json");
        for(auto it = jdata.begin(); it != jdata.end(); ++it) {
            const auto& key = it.key();
            const auto& value = it.value();
            eObjectInfo info;
            info.fSize = value.value("size", 1.f);
            info.fObstacle = value.value("obstacle", true);
            info.fWalkable = value.value("walkable", false);
            const auto typeStr = value.value("type", "");
            if(typeStr == "treasure") {
                info.fType = eObjectType::treasure;
            } else if(typeStr == "waypoint") {
                info.fType = eObjectType::waypoint;
            } else if(typeStr == "portal") {
                info.fType = eObjectType::portal;
            } else if(typeStr == "stash") {
                info.fType = eObjectType::stash;
            } else if(typeStr == "healer") {
                info.fType = eObjectType::healer;
            } else if(typeStr == "portalArea") {
                info.fType = eObjectType::portalArea;
            } else if(typeStr == "spawnArea") {
                info.fType = eObjectType::spawnArea;
            } else if(typeStr == "empty") {
                info.fType = eObjectType::empty;
            } else if(typeStr == ""){
                info.fType = eObjectType::none;
            } else {
                eRuntimeThrow("Unrecognized object type \"" + typeStr + "\"");
            }
            info.fTexStr = value.value("texture", "");
            sObjects.add(key, info);
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/objects.json");
    }
}

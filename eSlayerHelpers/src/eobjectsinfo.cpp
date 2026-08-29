#include "eSlayerHelpers/eobjectsinfo.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/esoundsbase.h"

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

            if(value.contains("size")) {
                const float size = value.value("size", 1.f);
                info.fWidth = size;
                info.fHeight = size;
            } else {
                info.fWidth = value.value("width", 1.f);
                info.fHeight = value.value("height", 1.f);
            }

            const auto blocksLightStr = value.value("blocksLight", "none");
            if(blocksLightStr == "none") {
                info.fBlocksLight = eBlockLightType::none;
            } else if(blocksLightStr == "center") {
                info.fBlocksLight = eBlockLightType::center;
            } else if(blocksLightStr == "rect") {
                info.fBlocksLight = eBlockLightType::rect;
            } else {
                eRuntimeThrow("Unrecognized block light type \"" +
                              blocksLightStr + "\" in " + dir + "/objects.json");
            }
            info.fShadow = value.value("shadow", true);
            info.fFlat = value.value("flat", false);
            info.fLightRadius = value.value("lightRadius", 0.f);
            info.fSplit = value.value("split", false);

            info.fObstacle = value.value("obstacle", true);
            info.fWalkable = value.value("walkable", false);
            const auto typeStr = value.value("type", "");
            if(typeStr == "treasure") {
                info.fType = eObjectType::treasure;

                if(value.contains("items")) {
                    for(const auto& jitem : value["items"]) {
                        auto& item = info.fItemDrops.emplace_back();
                        item.read(jitem);
                    }
                }
            } else if(typeStr == "waypoint") {
                info.fType = eObjectType::waypoint;
            } else if(typeStr == "portal") {
                info.fType = eObjectType::portal;
            } else if(typeStr == "stash") {
                info.fType = eObjectType::stash;
            } else if(typeStr == "portalArea") {
                info.fType = eObjectType::portalArea;
            } else if(typeStr == "spawnArea") {
                info.fType = eObjectType::spawnArea;
            } else if(typeStr == "trapDoor") {
                info.fType = eObjectType::trapDoor;
            } else if(typeStr == "portalDoor") {
                info.fType = eObjectType::portalDoor;
            } else if(typeStr == "message") {
                info.fType = eObjectType::message;
            } else if(typeStr == ""){
                info.fType = eObjectType::none;
            } else {
                eRuntimeThrow("Unrecognized object type \"" + typeStr + "\"");
            }
            const auto keyStr = value.value("key", "");
            if(!keyStr.empty()) {
                const int keyId = eItemsData::id(keyStr);
                if(keyId < 0) {
                    eRuntimeThrow("Invalid key type \"" + keyStr + "\".");
                }
                info.fKey = keyId;
            }
            info.fTexStr = value.value("texture", "");

            const auto parseSound = [&](const std::string& name) {
                const auto soundStr = value.value(name, "");
                if(soundStr.empty()) return -1;
                const int id = eSoundsBase::sSounds.id(soundStr);
                if(id < 0) {
                    eRuntimeThrow("Unrecognized sound \"" + soundStr + "\".");
                }
                return id;
            };

            info.fAppearSound = parseSound("appearSound");
            info.fTriggerSound = parseSound("triggerSound");

            sObjects.add(key, info);
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/objects.json");
    }
}

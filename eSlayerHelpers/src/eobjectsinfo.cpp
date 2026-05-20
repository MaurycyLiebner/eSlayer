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
            info.fObstacle = jdata.value("obstacle", true);
            const auto typeStr = value.value("type", "");
            if(typeStr == "treasure") {
                info.fType = eObjectType::treasure;
            } else {
                info.fType = eObjectType::none;
            }
            info.fTexStr = value.value("texture", "");
            sObjects.add(key, info);
        }
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/objects.json");
    }
}

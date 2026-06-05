#include "eobjectnames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eobjectsinfo.h>

eObjectNames eObjectNames::sInstance;

std::string eObjectNames::name(const int objectId) {
    return sInstance.mNames[objectId];
}

bool eObjectNames::load() {
    if(sInstance.mLoaded) return true;
    sInstance.mLoaded = true;
    return reload();
}

bool eObjectNames::reload() {
    sInstance.mNames.clear();
    const auto dir = "Objects";
    const auto strMap = eFileLoader::loadNames(dir, "names");
    for(const auto& it : strMap) {
        const auto id = eObjectsInfo::sObjects.id(it.first);
        sInstance.mNames[id] = it.second;
    }
    return true;
}

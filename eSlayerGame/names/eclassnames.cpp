#include "eclassnames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eclasses.h>

eClassNames eClassNames::sInstance;

std::string eClassNames::name(const int classId) {
    return sInstance.mNames[classId];
}

bool eClassNames::load() {
    if(sInstance.mLoaded) return true;
    sInstance.mLoaded = true;
    return reload();
}

bool eClassNames::reload() {
    sInstance.mNames.clear();
    const auto dir = "Classes";
    const auto strMap = eFileLoader::loadNames(dir, "names");
    for(const auto& it : strMap) {
        const auto id = eClasses::sClasses.id(it.first);
        sInstance.mNames[id] = it.second;
    }
    return true;
}

#include "eclassnames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/eclasses.h>

eClassNames eClassNames::sInstance;

std::string eClassNames::name(const int itemDataId) {
    return sInstance.mNames[itemDataId];
}

bool eClassNames::load() {
    const auto dir = "Classes";
    const auto strMap = eFileLoader::loadNames(dir, "names");
    for(const auto& it : strMap) {
        const auto id = eClasses::sClasses.id(it.first);
        sInstance.mNames[id] = it.second;
    }
    return true;
}

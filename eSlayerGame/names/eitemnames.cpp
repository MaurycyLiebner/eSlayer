#include "eitemnames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/eitemsdata.h>

eItemNames eItemNames::sInstance;

std::string eItemNames::name(const int itemDataId) {
    return sInstance.mNames[itemDataId];
}

bool eItemNames::load() {
    const auto dir = "Items";
    const auto strMap = eFileLoader::loadNames(dir, "names.txt");
    for(const auto& it : strMap) {
        const auto id = eItemsData::id(it.first);
        sInstance.mNames[id] = it.second;
    }
    return true;
}

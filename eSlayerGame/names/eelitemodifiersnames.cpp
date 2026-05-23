#include "eelitemodifiersnames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/eelitemodifiersinfo.h>

eEliteModifiersNames
eEliteModifiersNames::sInstance;

std::string eEliteModifiersNames::name(const int eliteId) {
    return sInstance.mNames[eliteId];
}

bool eEliteModifiersNames::load() {
    const auto dir = "Units";
    const auto nameStrMap = eFileLoader::loadNames(dir, "Elite/names.txt");
    for(const auto& it : nameStrMap) {
        const auto id = eEliteModifiersInfo::sElite.id(it.first);
        sInstance.mNames[id] = it.second;
    }
    return true;
}

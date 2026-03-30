#include "emonsternames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/echardatainfo.h>

eMonsterNames eMonsterNames::sInstance;

std::string eMonsterNames::name(const int itemDataId) {
    return sInstance.mNames[itemDataId];
}

bool eMonsterNames::load() {
    const auto dir = "Monsters";
    const auto strMap = eFileLoader::loadNames(dir, "names.txt");
    for(const auto& it : strMap) {
        const auto id = eCharDataInfo::id(it.first);
        sInstance.mNames[id] = it.second;
    }
    return true;
}

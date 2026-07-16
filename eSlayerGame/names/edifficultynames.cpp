#include "edifficultynames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/edifficulties.h>

eDifficultyNames eDifficultyNames::sInstance;

std::string eDifficultyNames::name(const int objectId) {
    return sInstance.mNames[objectId];
}

bool eDifficultyNames::load() {
    if(sInstance.mLoaded) return true;
    sInstance.mLoaded = true;
    return reload();
}

bool eDifficultyNames::reload() {
    sInstance.mNames.clear();
    const auto dir = "Other";
    const auto strMap = eFileLoader::loadNames(dir, "difficulties");
    for(const auto& it : strMap) {
        const auto id = eDifficulties::sDifficulties.id(it.first);
        sInstance.mNames[id] = it.second;
    }
    return true;
}

#include "eskilltreenames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eskilltrees.h>

eSkillTreeNames eSkillTreeNames::sInstance;

std::string eSkillTreeNames::name(const int itemDataId) {
    return sInstance.mNames[itemDataId];
}

bool eSkillTreeNames::load() {
    const auto dir = "Classes";
    const auto strMap = eFileLoader::loadNames(dir, "skillTrees/names");
    for(const auto& it : strMap) {
        const auto id = eSkillTrees::sTrees.id(it.first);
        sInstance.mNames[id] = it.second;
    }
    return true;
}

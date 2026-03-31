#include "eskillnames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/eskills.h>

eSkillNames eSkillNames::sInstance;

std::string eSkillNames::name(const int itemDataId) {
    return sInstance.mNames[itemDataId];
}

bool eSkillNames::load() {
    const auto dir = "Skills";
    const auto strMap = eFileLoader::loadNames(dir, "names.txt");
    for(const auto& it : strMap) {
        const auto id = eSkills::sSkills.id(it.first);
        sInstance.mNames[id] = it.second;
    }
    return true;
}

#include "eskillnames.h"

#include "../efileloader.h"

#include <eSlayerHelpers/eskills.h>

eSkillNames eSkillNames::sInstance;

std::string eSkillNames::name(const int skillId) {
    return sInstance.mNames[skillId];
}

std::string eSkillNames::description(const int skillId) {
    return sInstance.mDescriptions[skillId];
}

bool eSkillNames::load() {
    if(sInstance.mLoaded) return true;
    sInstance.mLoaded = true;
    return reload();
}

bool eSkillNames::reload() {
    sInstance.mNames.clear();
    sInstance.mDescriptions.clear();
    const auto dir = "Skills";
    const auto nameStrMap = eFileLoader::loadNames(dir, "names");
    for(const auto& it : nameStrMap) {
        const auto id = eSkills::sSkills.id(it.first);
        sInstance.mNames[id] = it.second;
    }
    const auto descStrMap = eFileLoader::loadNames(dir, "descriptions");
    for(const auto& it : descStrMap) {
        const auto id = eSkills::sSkills.id(it.first);
        sInstance.mDescriptions[id] = it.second;
    }
    return true;
}

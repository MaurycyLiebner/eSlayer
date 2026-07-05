#include "eSlayerHelpers/eattributes.h"

#include "eSlayerHelpers/eexceptions.h"
#include "eSlayerHelpers/efileloaderbase.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include <cmath>

uint8_t eAttributes::sMaxLevel = 99;
uint16_t eAttributes::sStatPointsPerLevel = 5;
uint16_t eAttributes::sSkillPointsPerLevel = 1;
std::vector<uint32_t>
eAttributes::sLevelExperience;

uint32_t eAttributes::nextLevelExp() const {
    if(fLevel == 0) return 0;
    const auto id = fLevel - 1;
    if(id >= sLevelExperience.size()) return 0;
    return sLevelExperience.at(id);
}

bool eAttributes::levelUp() {
    if(fLevel == sMaxLevel) return false;
    fLevel++;
    fExp = 0.f;
    fStatPoints += sStatPointsPerLevel;
    return true;
}

uint32_t eAttributes::totalPoints() const {
    return fStrength + fDexterity +
           fVitality + fEnergy +
           fStatPoints;
}

bool eAttributes::samePoints(
    const eAttributes& attrs1,
    const eAttributes& attrs2) {
    const auto t1 = attrs1.totalPoints();
    const auto t2 = attrs2.totalPoints();
    return t1 == t2;
}

void eAttributes::load() {
    const auto dir = "Other";

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "experience.json");
        sMaxLevel = jdata.value("maxLevel", 99);
        sLevelExperience.resize(sMaxLevel - 1);
        for(int i = 1; i < sMaxLevel; i++) {
            const auto iStr = std::to_string(i);
            sLevelExperience[i - 1] = jdata.value(iStr, sLevelExperience[i - 2]);
        }
    } catch(...) {
        eRuntimeThrow("Error while parsing Other/experience.json");
    }

    try {
        const auto jdata = eFileLoaderBase::parse(dir, "points.json");
        sSkillPointsPerLevel = jdata.value("skillPoints", 1);
        sStatPointsPerLevel = jdata.value("attributePoints", 5);
    } catch(...) {
        eRuntimeThrow("Error while parsing Other/points.json");
    }
}

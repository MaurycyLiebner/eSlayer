#include "eSlayerHelpers/eattributes.h"

#include "eSlayerHelpers/eunitsinfo.h"

uint32_t eAttributes::nextLevelExp(
    const int uinfoId) const {
    if(fLevel == 0) return 0;
    const auto id = fLevel - 1;
    const auto& uinfo = eUnitsInfo::sUnits.get(uinfoId);
    const auto& exps = uinfo.fLevelExperience;
    if(id >= exps.size()) return 0;
    return exps[id];
}

bool eAttributes::levelUp(
    const int uinfoId) {
    const auto& uinfo = eUnitsInfo::sUnits.get(uinfoId);
    const auto maxLevel = uinfo.fMaxLevel;
    if(fLevel >= maxLevel) return false;
    fLevel++;
    fExp = 0.f;
    addStatPoints(uinfo.fStatPointsPerLevel);
    return true;
}

void eAttributes::addStatPoints(const int count) {
    fStatPoints += count;
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

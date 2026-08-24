#include "eSlayerHelpers/eattributes.h"

#include "eSlayerHelpers/eunitsinfo.h"
#include "eSlayerHelpers/eclasses.h"

uint32_t eAttributes::nextLevelExp(
    const int uinfoId) const {
    if(fLevel == 0) return 0;
    const auto id = fLevel - 1;
    const auto& uinfo = eUnitsInfo::sUnits.get(uinfoId);
    const auto classId = uinfo.fClassId;
    const auto& class_ = eClasses::sClasses.get(classId);
    const auto& exps = class_.fLevelExperience;
    if(id >= exps.size()) return 0;
    return exps[id];
}

bool eAttributes::levelUp(
    const int uinfoId) {
    const auto& uinfo = eUnitsInfo::sUnits.get(uinfoId);
    const auto classId = uinfo.fClassId;
    const auto& class_ = eClasses::sClasses.get(classId);
    const auto maxLevel = class_.fMaxLevel;
    if(fLevel >= maxLevel) return false;
    fLevel++;
    fExp = 0.f;
    addStatPoints(class_.fStatPointsPerLevel);
    return true;
}

void eAttributes::addStatPoints(const int count) {
    fStatPoints += count;
}

void eAttributes::resetSkillStats(const int uinfoId) {
    const auto& uinfo = eUnitsInfo::sUnits.get(uinfoId);
    const auto classId = uinfo.fClassId;
    const auto& class_ = eClasses::sClasses.get(classId);

    const auto handle = [&](uint16_t& attr, const uint16_t ini) {
        fStatPoints += attr - ini;
        attr = ini;
    };

    handle(fStrength, class_.fIniStrength);
    handle(fDexterity, class_.fIniDexterity);
    handle(fVitality, class_.fIniVitality);
    handle(fEnergy, class_.fIniEnergy);
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

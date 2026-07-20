#include "eSlayerHelpers/eskilllevels.h"

#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/eclasses.h"

eSkillLevels::eSkillLevels() :
    std::map<uint16_t, uint16_t>{{0, 0}} {}

void eSkillLevels::read(ePacket& p) {
    p >> fRemainingPoints;
    uint16_t nSkills;
    p >> nSkills;
    for(int i = 0; i < nSkills; i++) {
        uint16_t skillId;
        p >> skillId;
        uint16_t level;
        p >> level;
        (*this)[skillId] = level;
    }
}

void eSkillLevels::write(ePacket& p) const {
    p << fRemainingPoints;
    const uint16_t nSkills = size();
    p << nSkills;
    for(const auto& skill : *this) {
        const uint16_t skillId = skill.first;
        p << skillId;
        const uint16_t level = skill.second;
        p << level;
    }
}

int eSkillLevels::skillLevel(const int skillId) const {
    const auto it = find(skillId);
    if(it == end()) return -1;
    return it->second;
}

void eSkillLevels::incSkillLevels(const int by) {
    for(auto& level : *this) {
        level.second += by;
    }
}

void eSkillLevels::incClassSkillLevels(const int classId, const int by) {
    const auto& class_ = eClasses::sClasses.get(classId);
    for(auto& level : *this) {
        const auto skillId = level.first;
        const bool r = class_.isClassSkill(skillId);
        if(!r) continue;
        level.second += by;
    }
}

void eSkillLevels::incSkillLevel(const int by, const int skillId) {
    (*this)[skillId] += by;
}

int eSkillLevels::totalPoints() const {
    int result = fRemainingPoints;
    for(const auto& skill : *this) {
        if(skill.first <= 0) continue;
        result += 1 + skill.second;
    }
    return result;
}
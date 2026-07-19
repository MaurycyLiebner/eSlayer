#include "eSlayerHelpers/eclass.h"

#include "eSlayerHelpers/eskilltrees.h"

bool eClass::isClassSkill(const int skillId) const {
    for(const auto treeId : fSkillTrees) {
        const auto& tree = eSkillTrees::sTrees.get(treeId);
        for(const auto& skill : tree.fSkills) {
            if(skill.fSkillId == skillId) return true;
        }
    }
    return false;
}
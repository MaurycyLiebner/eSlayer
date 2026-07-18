#include "eSlayerHelpers/eclass.h"

#include "eSlayerHelpers/eskilltrees.h"

bool eClass::isClassSkill(const int skillId) const {
    for(const auto treeId : fSkillTrees) {
        const auto& tree = eSkillTrees::sTrees.get(treeId);
        const auto r = tree.fSkills.count(skillId);
        if(r > 0) return true;
    }
    return false;
}
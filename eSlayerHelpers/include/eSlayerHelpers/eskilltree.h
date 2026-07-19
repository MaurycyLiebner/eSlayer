#ifndef ESKILLTREE_H
#define ESKILLTREE_H

#include "eslayerhelpersexport.h"

#include <set>
#include <vector>

struct eSkillTreeSkill {
    std::set<int> fPrerequisites;
    int fSkillId;
    int fLevelReq;
    int fColumn;
};

struct ESLAYERHELPERS_API eSkillTree {
    std::vector<eSkillTreeSkill> fSkills;
    int fMaxLevelReq;
};

#endif // ESKILLTREE_H

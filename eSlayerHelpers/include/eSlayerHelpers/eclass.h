#ifndef ECLASS_H
#define ECLASS_H

#include <vector>
#include <set>

struct eClass {
    std::set<int> fSkillTrees;
    std::vector<std::vector<int>> fIniItems;

    bool isClassSkill(const int skillId) const;
};

#endif // ECLASS_H

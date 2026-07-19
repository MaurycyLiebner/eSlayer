#ifndef ECLASS_H
#define ECLASS_H

#include <set>

#include "emodifier.h"

struct eIniItem {
    int fItemType;
    std::vector<eModifier> fMods;
};

struct eClass {
    std::set<int> fSkillTrees;
    std::vector<std::vector<eIniItem>> fIniItems;
    uint32_t fIniGold;

    bool isClassSkill(const int skillId) const;
};

#endif // ECLASS_H

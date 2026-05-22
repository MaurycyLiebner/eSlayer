#ifndef ESKILLTOTALMODS_H
#define ESKILLTOTALMODS_H

#include "emodifier.h"

#include <map>

struct eSkillLevel;

class eSkillTotalMods : public std::multimap<eModifierType, eModifier> {
public:
    void addLevel(const eSkillTotalMods& other);
    void add(const eModifier& mod);
    void addBoost(const eSkillLevel& boost);
    void collapse();

    int fCount = 0;
    float fManaCost = 0.f;
    float fCooldown = 0.f;
};

#endif // ESKILLTOTALMODS_H

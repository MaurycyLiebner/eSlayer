#ifndef ESKILLTOTALMODS_H
#define ESKILLTOTALMODS_H

#include "eslayerhelpersexport.h"

#include "emodifier.h"

#include <map>

struct eSkillLevelStats;

class ESLAYERHELPERS_API eSkillTotalMods :
    public std::multimap<eModifierType, eModifier> {
public:
    void addLevel(const eSkillTotalMods& other);
    void add(const eModifier& mod);
    void addBoost(const eSkillLevelStats& boost);
    void collapse();

    int fCount = 0;
    float fManaCost = 0.f;
    float fCooldown = 0.f;
    uint8_t fRadiusU = 0;
    float fRadius = 0.f;

    void setRadiusU(const uint8_t r);
};

#endif // ESKILLTOTALMODS_H

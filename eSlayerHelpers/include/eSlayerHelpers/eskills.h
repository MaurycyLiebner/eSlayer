#ifndef ESKILLS_H
#define ESKILLS_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"
#include "edamage.h"

enum class eSkillType {
    attack, missile
};

struct eSkillLevel {
    int fLevel;
    eDamage fDamage;
    int fMissiles;
    float fPierceChance;
};

struct eSkill {
    eSkillType fType;

    std::string fIcon;
    int fIconId;
    std::string fMissile;
    int fMissileId;
    std::string fPath;
    int fPathId;

    int fBaseMissiles;
    eDamage fBaseDamage;
    float fRange;
    float fRadius;
    float fSpeed;
    float fBasePierceChance;
    float fMaxAngle;
    std::vector<std::string> fCastAnims;
    std::vector<eSkillLevel> fLevels;
};

struct eUnitSkill : public eSkill {
    std::vector<int> fCastAnimIds;
};

class ESLAYERHELPERS_API eSkills {
public:
    static void load();

    static eStringIdMapVector<eSkill> sSkills;
private:
    static bool sLoaded;
};

#endif // ESKILLS_H

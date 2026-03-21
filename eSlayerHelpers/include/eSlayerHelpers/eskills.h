#ifndef ESKILLS_H
#define ESKILLS_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

enum class eSkillType {
    attack, missile
};

struct eSkill {
    eSkillType fType;
    std::string fIcon;
    int fIconId;
    std::string fMissile;
    int fMissileId;
    std::string fPath;
    int fPathId;
    float fRange;
    float fRadius;
    float fSpeed;
    float fPierceChance;
    std::vector<std::string> fAnims;
};

struct eSkillData {
    eSkillType fType;
    int fIconId;
    int fMissileId;
    int fPathId;
    float fRange;
    float fRadius;
    std::vector<int> fAnims;
};

class ESLAYERHELPERS_API eSkills {
public:
    static void load();

    static eStringIdMapVector<eSkill> sSkills;
private:
    static bool sLoaded;
};

#endif // ESKILLS_H

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
    std::string fMissile;
};

class ESLAYERHELPERS_API eSkills {
public:
    static void load();

    static eStringIdMapVector<eSkill> sSkills;
private:
    static bool sLoaded;
};

#endif // ESKILLS_H

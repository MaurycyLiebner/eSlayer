#ifndef ESKILLS_H
#define ESKILLS_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"
#include "emodifier.h"

enum class eSkillType : uint8_t {
    attack, smite, kick,
    shoot, throw_,
    missile, wall,
    summon,
    passive,
    aura,
    boostCurse
};

struct eSkillLevel {
    int fLevel;
    int fCount;
    float fCooldown;
    float fManaCost;
    std::map<eModifierType, eModifier>
    fTotalModifiers;
};

struct eSkill {
    eSkillType fType;

    std::string fIcon;
    int fIconId;
    std::string fMissileStr;
    int fMissileId;
    std::string fUnitStr;
    int fUnitId;
    std::string fPath;
    int fPathId;

    float fRangeTime;
    float fMissileEnemyFindRange;
    float fRadius;
    float fSpeed;
    float fTime;
    float fMaxAngle;
    float fCastRange;
    std::vector<eModifier> fModifiers;
    std::vector<std::string> fCastAnims;
    std::vector<eSkillLevel> fLevels;

    const eSkillLevel& skillLevel(const int skillLevelId) const {
        return fLevels[std::clamp(skillLevelId, 0, int(fLevels.size()) - 1)];
    }
};

struct eUnitSkill {
    int fSkillId;
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

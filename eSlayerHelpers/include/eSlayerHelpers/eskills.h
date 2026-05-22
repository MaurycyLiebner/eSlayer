#ifndef ESKILLS_H
#define ESKILLS_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"
#include "emodifier.h"
#include "eskilltotalmods.h"

enum class eSkillType : uint8_t {
    attack, smite, kick,
    shoot, throw_,
    missile, wall,
    nova,
    summon,
    passive,
    aura,
    boostCurse
};

struct eSkillLevel {
    int fLevel;

    eSkillTotalMods fModifiers;
    eSkillTotalMods fTotalModifiers;
};

struct eSynergy {
    std::string fSkillStr;
    int fSkillId;
    std::vector<eSkillLevel> fBoostLevels;

    const eSkillLevel& boostLevel(const int boostLevelId) const {
        return fBoostLevels[std::clamp(boostLevelId, 0, int(fBoostLevels.size()) - 1)];
    }
};

struct eSkill {
    eSkillType fType;

    std::string fIcon;
    int fIconId;
    std::string fMissileStr;
    int fMissileId;
    std::string fNovaStr;
    int fNovaId;
    std::string fUnitStr;
    int fUnitId;
    std::string fPath;
    int fPathId;

    const static float sRangeTimeMax;
    float fRangeTime;
    float fMissileEnemyFindRange;
    static const float sRadiusMax;
    float fRadius;
    static const float sSpeedMax;
    float fSpeed;
    float fTime;
    float fMaxAngle;
    bool fAngleAdjust;
    float fCastRange;
    std::vector<eModifier> fModifiers;
    std::vector<std::string> fCastAnims;
    std::vector<eSkillLevel> fLevels;
    std::vector<eSynergy> fSynergies;

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
    static const int sMaxSkillLevel;
private:
    static bool sLoaded;
};

#endif // ESKILLS_H

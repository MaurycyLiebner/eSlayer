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

struct eSkillLevelStats {
    int fLevel;

    eSkillTotalMods fModifiers;
    eSkillTotalMods fTotalModifiers;
};

struct eSkillLevelsStats : public std::vector<eSkillLevelStats> {
    const eSkillLevelStats& skillLevel(const int skillLevelId) const {
        return (*this)[std::clamp(skillLevelId, 0, int(size()) - 1)];
    }
};

struct eSynergy {
    std::string fSkillStr;
    int fSkillId;
    eSkillLevelsStats fBoostLevels;

    const eSkillLevelStats& boostLevel(const int boostLevelId) const {
        return fBoostLevels.skillLevel(boostLevelId);
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
    eSkillLevelsStats fLevels;
    std::vector<eSynergy> fSynergies;

    const eSkillLevelStats& skillLevel(const int skillLevelId) const {
        return fLevels.skillLevel(skillLevelId);
    }
};

struct eUnitSkill {
    int fSkillId;
    std::vector<int> fCastAnimIds;
};

class ESLAYERHELPERS_API eSkills {
public:
    static void load();

    static eSkillLevelStats parseSkillLevel(
        const ordered_json& levelData,
        eSkillTotalMods& totalMods);
    static void parseSkillLevels(
        const ordered_json& levelsJson,
        std::vector<eSkillLevelStats>& levels,
        const int count = 0,
        const float cooldown = 0.f,
        const float manaCost = 0.f);

    static eStringIdMapVector<eSkill> sSkills;
    static const int sMaxSkillLevel;
private:
    static bool sLoaded;
};

#endif // ESKILLS_H

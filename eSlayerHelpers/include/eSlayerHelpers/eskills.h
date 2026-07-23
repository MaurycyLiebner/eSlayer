#ifndef ESKILLS_H
#define ESKILLS_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"
#include "emodscollection.h"

enum class eSkillType : uint8_t {
    attack, dualAttack, smite, kick,
    shoot, throw_,
    missile, wall,
    nova,
    summon,
    passive,
    aura,
    boostCurse,
    area
};

struct eSynergy {
    std::string fSkillStr;
    int fSkillId;
    eModsCollectionLevels fBoostLevels;

    const eModsCollectionLevel& boostLevel(const int boostLevelId) const {
        return fBoostLevels.skillLevel(boostLevelId);
    }
};

enum class eBoostCurseType : uint8_t;
enum class eBoostCurseTarget;

enum class eAuraType;
enum class eAuraTarget;

struct eSkill {
    eSkillType fType;
    bool fTargetCorpse;

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

    eBoostCurseType fBoostCurseType;
    eBoostCurseTarget fBoostCurseTarget;

    eAuraType fAuraType;
    eAuraTarget fAuraTarget;

    std::string fAreaMissileStr;
    int fAreaMissileId;

    std::string fSelfAreaMissileStr;
    int fSelfAreaMissileId;

    const static float sRangeMax;
    float fRange;
    float fMissileEnemyFindRange;
    static const float sRadiusMax;
    static const float sSpeedMax;
    float fSpeed;
    const static float sTimeMax;
    float fTime;
    float fBoostCurseTime;
    float fMaxAngle;
    bool fAngleAdjust;
    float fCastRange;
    std::vector<std::string> fCastAnims;
    eModsCollectionLevels fLevels;
    std::vector<eSynergy> fSynergies;

    const eModsCollectionLevel& skillLevel(const int skillLevelId) const {
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

    static eStringIdMapVector<eSkill> sSkills;
    static const int sMaxSkillLevel;
private:
    static bool sLoaded;
};

#endif // ESKILLS_H

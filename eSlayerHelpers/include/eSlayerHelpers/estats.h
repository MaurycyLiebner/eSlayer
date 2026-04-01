#ifndef ESTATS_H
#define ESTATS_H

#include "eslayerhelpersexport.h"

#include "edamage.h"
#include "eweapontype.h"
#include "eskillchoice.h"

#include <map>
#include <vector>

struct eEquipment;
struct eAttributes;

struct eSkillStats {
    int fSkillId = -1;

    eDamage fDamageMinLW;
    eDamage fDamageMaxLW;
    eDamage fDamageMinRW;
    eDamage fDamageMaxRW;

    float fPierceLW = 0.f;
    float fPierceRW = 0.f;

    float fAttackSpeedS = 0.f;

    float fAttackRatingLW = 0.f;
    float fAttackRatingRW = 0.f;

    int fMissileIdLW = -1;
    int fMissileIdRW = -1;

    float fMissileRangeTime = 0.f;

    int fMissiles = 0;
};

struct ESLAYERHELPERS_API eStats {
    float fExperience = 0.f;

    std::vector<eSkillStats> fSkills;

    float fDefense = 100.f;
    float fBlockChance = 0.f;
    float fFasterHitRecovery = 0.f;
    float fFasterBlockRate = 0.f;
    float fWalkRun = 0.f;

    float fStamina = 100.f;

    float fHealthF = 100.f;
    float fMaxHealth = 100.f;

    float fManaF = 100.f;
    float fMaxMana = 100.f;

    float fStrength = 0.f;
    float fDexterity = 0.f;
    float fVitality = 0.f;
    float fEnergy = 0.f;

    float fAttackSpeedLW = 0.f;
    float fAttackSpeedRW = 0.f;

    float fCastRate = 0.f;

    float fWeaponMeeleRange = 0.f;
    float fWeaponRangedRange = 0.f;

    float fFireResistance = 0.f;
    float fColdResistance = 0.f;
    float fLightningResistance = 0.f;
    float fPoisonResistance = 0.f;

    float fMaxFireResistance = 0.75f;
    float fMaxColdResistance = 0.75f;
    float fMaxLightningResistance = 0.75f;
    float fMaxPoisonResistance = 0.75f;

    eWeaponType fWeaponTypeL = eWeaponType::none;
    eWeaponType fWeaponTypeR = eWeaponType::none;

    std::map<int, float> fCooldowns;
    std::map<int, int> fSkillLevels;

    eSkillStats& leftSkill() { return fSkills[0]; }
    eSkillStats& rightSkill() { return fSkills[1]; }

    eSkillStats& skill(const eSkillChoice schoice);
    const eSkillStats& skill(const eSkillChoice schoice) const;

    const eSkillStats& leftSkill() const { return fSkills[0]; }
    const eSkillStats& rightSkill() const { return fSkills[1]; }

    void calculate(const eAttributes& attr,
                   const eEquipment& eq);

    bool canUseSkill(const eSkillChoice schoice) const;
    bool rangedAttack(const eSkillChoice schoice) const;
};

#endif // ESTATS_H

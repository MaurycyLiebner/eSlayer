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
class ePacket;

struct eSkillStats {
    int fSkillId = 0;

    eDamage fDamageMinLW;
    eDamage fDamageMaxLW;
    eDamage fDamageMinRW;
    eDamage fDamageMaxRW;

    float fPierceLW = 0.f;
    float fPierceRW = 0.f;

    float fAttackSpeedS = 0.f;

    bool fKnockbackLW = false;
    bool fKnockbackRW = false;

    float fAttackRatingLW = 0.f;
    float fAttackRatingRW = 0.f;

    int fMissileIdLW = -1;
    int fMissileIdRW = -1;

    float fMissileRangeTime = 0.f;

    float fLifeStealLW = 0.f;
    float fLifeStealRW = 0.f;

    float fManaStealLW = 0.f;
    float fManaStealRW = 0.f;

    float fMeeleSplashDamageLW = 0.f;
    float fMeeleSplashDamageRW = 0.f;

    int fCount = 0;
};

struct ESLAYERHELPERS_API eSkillLevels : public std::map<uint16_t, uint16_t> {
    eSkillLevels() : std::map<uint16_t, uint16_t>{{0, 0}} {}

    uint8_t fRemainingPoints = 10;

    void read(ePacket& p);
    void write(ePacket& p) const;

    int skillLevel(const int skillId) const;
    void incSkillLevels(const int by);
};

struct ESLAYERHELPERS_API eStats {
    std::vector<eSkillStats> fSkills;

    int fClass = 0;

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

    // Weapon speed modifier
    float fWSMLW = 0.f;
    float fWSMRW = 0.f;

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
    eSkillLevels fSkillLevels;

    eSkillStats& leftSkill();
    eSkillStats& rightSkill();

    const eSkillStats& leftSkill() const;
    const eSkillStats& rightSkill() const;

    eSkillStats& skill(const eSkillChoice schoice);
    const eSkillStats& skill(const eSkillChoice schoice) const;

    eSkillStats& skill(const int schoice);
    const eSkillStats& skill(const int schoice) const;

    void calculate(const eAttributes& attr,
                   const eEquipment& eq);

    bool canUseSkill(const eSkillChoice schoice) const;
    bool rangedAttack(const eSkillChoice schoice) const;
    float attackRange(const eSkillChoice schoice,
                      const float unit1Radius,
                      const float unit2Radius) const;

    bool canUseSkill(const int schoice) const;
    bool rangedAttack(const int schoice) const;
    float attackRange(const int schoice,
                      const float unit1Radius,
                      const float unit2Radius) const;
    bool attackRangeSkill(const float minRange,
                          int& resultSchoice,
                          const float unit1Radius,
                          const float unit2Radius) const;
    bool attackRangeSkill(const float minRange,
                          int& resultSchoice,
                          const float unit1Radius,
                          const float unit2Radius,
                          std::vector<int>& schoices) const;
    float maxRangeSkill(int& resultSchoice,
                        const float unit1Radius,
                        const float unit2Radius) const;
    int skillLevel(const int skillId) const;
    int incSkillLevel(const int skillId);
};

#endif // ESTATS_H

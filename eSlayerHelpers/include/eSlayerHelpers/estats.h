#ifndef ESTATS_H
#define ESTATS_H

#include "emodifier.h"
#include "edamage.h"
#include "eskillchoice.h"
#include "eslayerhelpersexport.h"
#include "eweapontype.h"

#include <map>
#include <vector>

struct eEquipment;
struct eAttributes;
class ePacket;
struct eItem;
enum class eWeaponChoice : uint8_t;

struct eSkillStats {
    int fSkillId = 0;

    // for chance casting
    float fCastChance = 0.f;
    int fSkillLevelId = -1;

    int fCount = 0;
    float fManaCost = 0.f;
    float fCooldown = 0.f;

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

    float fColdLengthLW = 0.f;
    float fColdLengthRW = 0.f;

    float fFreezeLengthLW = 0.f;
    float fFreezeLengthRW = 0.f;

    std::vector<eSkillStats> fOnAttackLW;
    std::vector<eSkillStats> fOnAttackRW;
    std::vector<eSkillStats> fOnStrikingLW;
    std::vector<eSkillStats> fOnStrikingRW;
    std::vector<eSkillStats> fOnKillLW;
    std::vector<eSkillStats> fOnKillRW;
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

    std::vector<eSkillStats> fOnStruck;
    std::vector<eSkillStats> fOnDeath;

    int fClass = 0;
    int fLevel = 1;

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

    // per frame
    float fHealthRegeneration = 0.f;
    float fManaRegeneration = 0.f;

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
    float fPhysicalResistance = 0.f;

    float fMaxFireResistance = 0.75f;
    float fMaxColdResistance = 0.75f;
    float fMaxLightningResistance = 0.75f;
    float fMaxPoisonResistance = 0.75f;
    float fMaxPhysicalResistance = 0.75f;

    eWeaponType fWeaponTypeL = eWeaponType::none;
    eWeaponType fWeaponTypeR = eWeaponType::none;

    std::map<int, float> fCooldowns;
    eSkillLevels fBaseSkillLevels;
    eSkillLevels fEffectiveSkillLevels;

    std::vector<eModifier> fBoosts;

    bool canUseSkill(const int schoice,
                     const eWeaponChoice wchoice) const;

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

    void calculateSkill(const int schoice,
                        const eEquipment& eq);
    void calculateSkill(eSkillStats& stats,
                        const eEquipment& eq,
                        const bool chanceSkill) const;

    eSkillStats statsFromMod(const eModifier& mod,
                             const eEquipment& eq) const;

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
    int effectiveSkillLevel(const int skillId) const;
    int baseSkillLevel(const int skillId) const;
    int incSkillLevel(const int skillId);
    float manaCost(const int schoice) const;
    float cooldown(const int schoice) const;
    bool itemReqsMet(const eItem& item) const;
};

#endif // ESTATS_H

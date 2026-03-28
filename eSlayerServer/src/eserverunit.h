#ifndef ESERVERUNIT_H
#define ESERVERUNIT_H

#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/edamage.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/eskillchoice.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eweaponchoice.h>
#include <eSlayerHelpers/eweapontype.h>

#include <memory>

class eUnitAction;
class eComplexAction;
class eCharData;
class eHitData;

class eServerUnit : public eUnitData {
public:
    static int sNextCharId;

    eServerUnit(const eCharData& data);

    bool aggressive() const { return mAggressive; }

    float level() const { return mLevel; }

    float attackRatingL() const { return mAttackRatingL; }
    float attackRatingR() const { return mAttackRatingR; }
    float defense() const;

    float blockChance() const;

    float fasterHitRecovery() const { return mFasterHitRecovery; }

    float fasterBlockRate() const { return mFasterBlockRate; }

    float weaponMeeleRange() const { return mWeaponMeeleRange; }

    eWeaponType weaponTypeL() const { return mWeaponTypeL; }
    eWeaponType weaponTypeR() const { return mWeaponTypeR; }
    eWeaponType weaponType(const eWeaponChoice wchoice) const;

    int missileIdLWLS() const { return mMissileIdLWLS; }
    int missileIdRWLS() const { return mMissileIdRWLS; }
    int missileIdLWRS() const { return mMissileIdLWRS; }
    int missileIdRWRS() const { return mMissileIdRWRS; }

    int missileId(const eWeaponChoice wchoice,
                  const eSkillChoice schoice) const;

    float weaponRangedRange() const { return mWeaponRangedRange; }

    eWeaponData weaponData() const;

    void setEquipment(const eEquipment& eq);
    eEquipment& equipment() { return mEquipment; }

    float itemsAttackSpeed(const eWeaponChoice wchoice) const;
    float skillsAttackSpeed(const eSkillChoice schoice) const;
    float weaponSpeedModifier(const eWeaponChoice wchoice) const;
    float itemsCastRate() const { return mCastRate; }

    static float sHitChance(const eServerUnit& hit,
                            const eServerUnit& by,
                            const eSkillChoice schoice);
    int attackMissiles(const eSkillChoice schoice,
                       const eWeaponChoice wchoice);
    float pierceChance(const eSkillChoice schoice,
                       const eWeaponChoice wchoice);

    bool getHit(const eHitData& data);
    float takeDamage(const eDamage& dmg);
    eDamage attackDamage(const eSkillChoice schoice,
                         const eWeaponChoice wchoice);

    float maxHealth() const { return mMaxHealth; }

    void increment(const float by);
    int skillId(const eSkillChoice schoice) const;
    int skillLevel(const int skillId) const;
    bool skillReady(const eSkillChoice schoice) const;
    void useSkill(const eSkillChoice schoice);
    void setSkillId(const eSkillChoice schoice,
                    const int skillId);

    void setAction(const std::shared_ptr<eComplexAction>& a);
    void setChildAction(const std::shared_ptr<eUnitAction>& a);
    const std::shared_ptr<eComplexAction>&
    action() const { return mAction; }

    eMovementHandler& movementHandler()
    { return mHandler; }
    const eMovementHandler& movementHandler() const
    { return mHandler; }

    const eCharData& data() const { return mData; }

    std::vector<int> castAnims(const eSkillChoice schoice) const;

    eWeaponChoice useWeapon(const eSkillChoice schoice);

    bool canUseSkill(const eSkillChoice schoice,
                     const eWeaponChoice wchoice) const;
private:
    void recalculateStats();

    const eCharData& mData;
    bool mAggressive = false;
    std::shared_ptr<eComplexAction> mAction;
    eMovementHandler mHandler;

    float mLevel = 1.f;
    float mAttackRatingL = 100.f;
    float mAttackRatingR = 100.f;
    float mDefense = 100.f;
    float mBlockChance = 0.f;
    float mFasterHitRecovery = 0.f;
    float mFasterBlockRate = 0.f;
    float mWalkRun = 0.f;

    float mHealthF = 100.f;
    float mMaxHealth = 100.f;

    float mManaF = 100.f;
    float mMaxMana = 100.f;

    // LWLS - left weapon, left skill
    float mAttackSpeedLW = 0.f;
    float mAttackSpeedRW = 0.f;
    float mAttackSpeedLS = 0.f;
    float mAttackSpeedRS = 0.f;

    float mCastRate = 0.f;

    float mWeaponMeeleRange = 0.f;
    float mWeaponRangedRange = 0.f;

    eWeaponType mWeaponTypeL = eWeaponType::meele;
    eWeaponType mWeaponTypeR = eWeaponType::meele;

    int mMissileIdLWLS = -1;
    int mMissileIdRWLS = -1;
    int mMissileIdLWRS = -1;
    int mMissileIdRWRS = -1;

    // LWLS - left weapon, left skill
    eDamage mDamageMinLWLS;
    eDamage mDamageMaxLWLS;
    eDamage mDamageMinRWLS;
    eDamage mDamageMaxRWLS;

    eDamage mDamageMinLWRS;
    eDamage mDamageMaxLWRS;
    eDamage mDamageMinRWRS;
    eDamage mDamageMaxRWRS;

    eEquipment mEquipment;

    std::map<int, float> mCooldowns;
    std::map<int, int> mSkillLevels;

    int mSkillL = -1;
    int mSkillR = -1;

    int mMissilesL = 0;
    int mMissilesR = 0;

    float mPierceL = 0.f;
    float mPierceR = 0.f;

    eWeaponChoice mLastUsedWeapon = eWeaponChoice::left;
};

#endif // ESERVERUNIT_H

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
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/estats.h>

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

    float level() const { return mAttributes.fLevel; }

    float attackRatingL() const { return mStats.fAttackRatingL; }
    float attackRatingR() const { return mStats.fAttackRatingR; }
    float defense() const;

    float blockChance() const;

    float fasterHitRecovery() const { return mStats.fFasterHitRecovery; }

    float fasterBlockRate() const { return mStats.fFasterBlockRate; }

    float weaponMeeleRange() const { return mStats.fWeaponMeeleRange; }

    eWeaponType weaponTypeL() const { return mStats.fWeaponTypeL; }
    eWeaponType weaponTypeR() const { return mStats.fWeaponTypeR; }
    eWeaponType weaponType(const eWeaponChoice wchoice) const;

    int missileIdLWLS() const { return mStats.fMissileIdLWLS; }
    int missileIdRWLS() const { return mStats.fMissileIdRWLS; }
    int missileIdLWRS() const { return mStats.fMissileIdLWRS; }
    int missileIdRWRS() const { return mStats.fMissileIdRWRS; }

    int missileId(const eWeaponChoice wchoice,
                  const eSkillChoice schoice) const;

    float weaponRangedRange() const { return mStats.fWeaponRangedRange; }

    eWeaponData weaponData() const;

    void setEquipment(const eEquipment& eq);
    eEquipment& equipment() { return mEquipment; }

    float itemsAttackSpeed(const eWeaponChoice wchoice) const;
    float skillsAttackSpeed(const eSkillChoice schoice) const;
    float weaponSpeedModifier(const eWeaponChoice wchoice) const;
    float itemsCastRate() const { return mStats.fCastRate; }

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

    float maxHealth() const { return mStats.fMaxHealth; }

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

    eStats mStats;
    eAttributes mAttributes;
    eEquipment mEquipment;

    eWeaponChoice mLastUsedWeapon = eWeaponChoice::left;
};

#endif // ESERVERUNIT_H

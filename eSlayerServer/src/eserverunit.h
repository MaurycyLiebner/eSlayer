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
class eServerArea;

struct ePoisonDamage {
    float fPerFrame = 0.f;
    float fFrameLength = 0.f;
};

class eServerUnit : public eUnitData {
public:
    static int sNextCharId;

    eServerUnit(const eCharData& data,
                eServerArea& area);

    bool aggressive() const { return mAggressive; }

    float level() const { return mAttributes.fLevel; }

    float defense() const;

    float blockChance() const;

    float fasterHitRecovery() const { return mStats.fFasterHitRecovery; }

    float fasterBlockRate() const { return mStats.fFasterBlockRate; }

    float weaponMeeleRange() const { return mStats.fWeaponMeeleRange; }

    eWeaponType weaponTypeL() const { return mStats.fWeaponTypeL; }
    eWeaponType weaponTypeR() const { return mStats.fWeaponTypeR; }
    eWeaponType weaponType(const eWeaponChoice wchoice) const;

    int missileId(const eWeaponChoice wchoice,
                  const eSkillChoice schoice) const;
    float missileRangeTime(const eWeaponChoice wchoice,
                           const eSkillChoice schoice) const;

    float weaponRangedRange() const { return mStats.fWeaponRangedRange; }

    void setEquipment(const eEquipment& eq);
    eEquipment& equipment() { return mEquipment; }

    void setAttributes(const eAttributes& attrs);
    eAttributes& attributes() { return mAttributes; }

    float itemsAttackSpeed(const eWeaponChoice wchoice) const;
    float skillsAttackSpeed(const eSkillChoice schoice) const;
    float weaponSpeedModifier(const eWeaponChoice wchoice) const;
    float itemsCastRate() const { return mStats.fCastRate; }

    static float sHitChance(const eServerUnit& hit,
                            const eServerUnit& by,
                            const eSkillChoice schoice,
                            const eWeaponChoice wchoice);
    int attackMissiles(const eSkillChoice schoice,
                       const eWeaponChoice wchoice);
    float pierceChance(const eSkillChoice schoice,
                       const eWeaponChoice wchoice);

    bool getHit(const eHitData& data);
    float takeDamage(const eDamage& dmg);
    bool consumeMana(const float mana);
    eDamage attackDamage(const eSkillChoice schoice,
                         const eWeaponChoice wchoice);

    float mana() const { return mStats.fManaF; }
    float experience() const { return mStats.fExperience; }
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

    void killed(const eServerUnit& killed);
    void respawn();

    void recalculateStats();
private:
    const eCharData& mData;
    eServerArea& mArea;

    bool mAggressive = false;
    std::shared_ptr<eComplexAction> mAction;
    eMovementHandler mHandler;

    eStats mStats;
    eAttributes mAttributes;
    eEquipment mEquipment;

    eWeaponChoice mLastUsedWeapon = eWeaponChoice::left;

    std::vector<ePoisonDamage> mPoison;
};

#endif // ESERVERUNIT_H

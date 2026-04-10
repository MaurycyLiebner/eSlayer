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

    eServerUnit(const bool client,
                const eCharData& data,
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
                  const int schoice) const;
    float missileRangeTime(const eWeaponChoice wchoice,
                           const int schoice) const;

    float weaponRangedRange() const { return mStats.fWeaponRangedRange; }

    const eStats& stats() const { return mStats; }

    void setEquipment(const eEquipment& eq,
                      const bool recalc = true);
    eEquipment& equipment() { return mEquipment; }

    void setAttributes(const eAttributes& attrs,
                       const bool recalc = true);
    eAttributes& attributes() { return mAttributes; }

    void setSkillLevels(const eSkillLevels& skillLevels,
                        const bool recalc = true);

    float itemsAttackSpeed(const eWeaponChoice wchoice) const;
    float skillsAttackSpeed(const int schoice) const;
    float weaponSpeedModifier(const eWeaponChoice wchoice) const;
    float itemsCastRate() const { return mStats.fCastRate; }

    bool knockback(const int schoice,
                   const eWeaponChoice wchoice) const;
    bool alwaysHit(const int schoice,
                   const eWeaponChoice wchoice) const;
    float attackRating(const int schoice,
                       const eWeaponChoice wchoice) const;
    static float sHitChance(const eServerUnit& hit,
                            const eServerUnit& by,
                            const int schoice,
                            const eWeaponChoice wchoice);
    static float sHitChance(const eServerUnit& hit,
                            const eServerUnit& by,
                            const float ar);
    static float sHitChance(const eServerUnit& hit,
                            const float alvl,
                            const float ar);
    float lifeSteal(const int schoice,
                    const eWeaponChoice wchoice) const;
    float manaSteal(const int schoice,
                    const eWeaponChoice wchoice) const;

    float meeleSplashDamage(const int schoice,
                            const eWeaponChoice wchoice) const;

    std::vector<eModifier> skillModifiers(
        const int schoice,
        const eWeaponChoice wchoice) const;
    int skillCount(const int schoice,
                   const eWeaponChoice wchoice) const;
    float pierceChance(const int schoice,
                       const eWeaponChoice wchoice) const;

    bool getHit(const eHitData& data, const bool splash = true);
    float takeDamage(const eDamage& dmg);

    void restoreHealth(const float by);
    void restoreMana(const float by);

    bool consumeMana(const float mana);
    eDamage attackDamage(const int schoice,
                         const eWeaponChoice wchoice);

    float mana() const { return mStats.fManaF; }
    float experience() const { return mAttributes.fExp; }
    float maxHealth() const { return mStats.fMaxHealth; }

    void increment(const float by);
    int skillId(const eSkillChoice schoice) const;
    int skillId(const int schoice) const;
    bool skillReady(const eSkillChoice schoice) const;
    bool skillReady(const int schoice) const;
    void useSkill(const eSkillChoice schoice);
    void useSkill(const int schoice);
    void setSkillId(const eSkillChoice schoice,
                    const int skillId,
                    const bool recalc = true);
    void setSkillId(const int schoice,
                    const int skillId,
                    const bool recalc = true);
    void setBoosts(const std::vector<eModifier>& mods,
                   const bool recalc = true);
    void addBoost(const eModifier& mod,
                  const bool recalc = true);

    void setAction(const std::shared_ptr<eComplexAction>& a);
    void setChildAction(const std::shared_ptr<eUnitAction>& a);
    const std::shared_ptr<eComplexAction>&
    action() const { return mAction; }

    eMovementHandler& movementHandler()
    { return mHandler; }
    const eMovementHandler& movementHandler() const
    { return mHandler; }

    const eCharData& data() const { return mData; }

    std::vector<int> castAnims(const int schoice) const;

    eWeaponChoice useWeapon(const int schoice);

    bool canUseSkill(const int schoice,
                     const eWeaponChoice wchoice) const;

    void killed(const eServerUnit& killed);
    void respawn();

    void recalculateStats();

    int addSkill();

    std::vector<int> readySkills() const;

    const std::vector<int>& followers() const
    { return mFollowers; }
    std::vector<int>& followers()
    { return mFollowers; }
    std::vector<int> followers(const int charDataId) const;
    int countFollowers(const int charDataId) const;
private:
    const eCharData& mData;
    eServerArea& mArea;
    const bool mClient;

    bool mAggressive = false;
    std::shared_ptr<eComplexAction> mAction;
    eMovementHandler mHandler;

    eStats mStats;
    eAttributes mAttributes;
    eEquipment mEquipment;

    eWeaponChoice mLastUsedWeapon = eWeaponChoice::left;

    std::vector<ePoisonDamage> mPoison;
    std::vector<int> mFollowers;
};

#endif // ESERVERUNIT_H

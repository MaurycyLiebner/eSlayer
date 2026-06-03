#ifndef ESERVERUNIT_H
#define ESERVERUNIT_H

#include "actions/ecomplexaction.h"

#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/edamage.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/epotiontype.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eskillchoice.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eweaponchoice.h>
#include <eSlayerHelpers/eweapontype.h>

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

struct ePotionHealing {
    float fPerFrame = 0.f;
    float fFrameLength = 0.f;
};

enum eUnitType {
    normal,
    uniqueBoss,
    minion
};

struct eTimedBoost {
    eTimedBoost(const eBoostCurseType type,
                const int missileId,
                const float time) :
        fType(type),
        fMissileId(missileId),
        fRemTime(time) {}

    eBoostCurseType fType;
    int fMissileId;
    float fRemTime;
};

class eServerUnit : public eUnitData {
public:
    static int sNextCharId;

    eServerUnit(const bool slayer,
                const eCharData& data,
                const int unitTypeId,
                eServerArea& area,
                ePathFinderMap& map);

    int unitTypeId() const { return mUnitTypeId; }

    bool aggressive() const { return mAggressive; }

    int level() const { return mAttributes.fLevel; }

    bool hitData(const eSkillStats& skill,
                 const eWeaponChoice wchoice,
                 eHitData& data) const;

    float defense() const;

    float blockChance() const;

    float fasterHitRecovery() const { return mStats.fFasterHitRecovery; }

    float fasterBlockRate() const { return mStats.fFasterBlockRate; }

    float weaponMeeleRange() const { return mStats.fWeaponMeeleRange; }

    eWeaponType weaponTypeL() const { return mStats.fWeaponTypeL; }
    eWeaponType weaponTypeR() const { return mStats.fWeaponTypeR; }
    eWeaponType weaponType(const eWeaponChoice wchoice) const;

    int missileId(const int schoice,
                  const eWeaponChoice wchoice) const;
    static int missileId(const eSkillStats& stats,
                         const eWeaponChoice wchoice);
    float missileRange(const int schoice,
                       const eWeaponChoice wchoice) const;
    static float missileRange(const eSkillStats& stats,
                              const eWeaponChoice wchoice);
    float missileTime(const int schoice,
                      const eWeaponChoice wchoice) const;
    static float missileTime(const eSkillStats& stats,
                             const eWeaponChoice wchoice);
    float radius(const int schoice,
                 const eWeaponChoice wchoice) const;
    static float radius(const eSkillStats& stats,
                        const eWeaponChoice wchoice);

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
    void consumePotion(const uint32_t itemId);

    float itemsAttackSpeed(const eWeaponChoice wchoice) const;
    float skillsAttackSpeed(const int schoice) const;
    float weaponSpeedModifier(const eWeaponChoice wchoice) const;
    float itemsCastRate() const { return mStats.fCastRate; }

    bool knockback(const int schoice,
                   const eWeaponChoice wchoice) const;
    static bool knockback(const eSkillStats& stats,
                          const eWeaponChoice wchoice);
    bool alwaysHit(const int schoice,
                   const eWeaponChoice wchoice) const;
    static bool alwaysHit(const eSkillStats& stats,
                          const eWeaponChoice wchoice);
    float attackRating(const int schoice,
                       const eWeaponChoice wchoice) const;
    static float attackRating(const eSkillStats& stats,
                              const eWeaponChoice wchoice);
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
    static float lifeSteal(const eSkillStats& stats,
                           const eWeaponChoice wchoice);
    float manaSteal(const int schoice,
                    const eWeaponChoice wchoice) const;
    static float manaSteal(const eSkillStats& stats,
                           const eWeaponChoice wchoice);

    float coldLength(const int schoice,
                     const eWeaponChoice wchoice) const;
    static float coldLength(const eSkillStats& stats,
                            const eWeaponChoice wchoice);
    float freezeLength(const int schoice,
                       const eWeaponChoice wchoice) const;
    static float freezeLength(const eSkillStats& stats,
                              const eWeaponChoice wchoice);

    std::vector<eSkillStats> onAttack(
        const int schoice, const eWeaponChoice wchoice) const;
    static std::vector<eSkillStats> onAttack(
        const eSkillStats& stats, const eWeaponChoice wchoice);

    std::vector<eSkillStats> onStriking(
        const int schoice, const eWeaponChoice wchoice) const;
    static std::vector<eSkillStats> onStriking(
        const eSkillStats& stats, const eWeaponChoice wchoice);

    std::vector<eSkillStats> onKill(
        const int schoice, const eWeaponChoice wchoice) const;
    static std::vector<eSkillStats> onKill(
        const eSkillStats& stats, const eWeaponChoice wchoice);
    std::vector<eBoostCurse> boosts(
        const eSkillStats& stats, const eWeaponChoice wchoice) const;

    float meeleSplashDamage(const int schoice,
                            const eWeaponChoice wchoice) const;
    static float meeleSplashDamage(const eSkillStats& stats,
                                   const eWeaponChoice wchoice);

    std::vector<eModifier> skillModifiers(
        const int schoice,
        const eWeaponChoice wchoice) const;
    int skillCount(const int schoice,
                   const eWeaponChoice wchoice) const;
    static int skillCount(const eSkillStats& stats,
                          const eWeaponChoice wchoice);
    float pierceChance(const int schoice,
                       const eWeaponChoice wchoice) const;
    static float pierceChance(const eSkillStats& stats,
                              const eWeaponChoice wchoice);

    bool getHit(const eHitData& data, const bool splash = true);
    float takeDamage(const eDamage& dmg);

    void restoreHealth(const float by);
    void restoreMana(const float by);

    bool consumeMana(const float mana);
    eDamage attackDamage(const int schoice,
                         const eWeaponChoice wchoice);
    static eDamage attackDamage(const eSkillStats& stats,
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
    void addBoost(const std::vector<eModifier>& mods,
                  const eBoostCurseType type = eBoostCurseType::regular,
                  const bool recalc = true);
    void removeBoost(const eBoostCurseType type,
                     const bool recalc = true);
    void addTimedBoost(const std::vector<eModifier>& mods,
                       const eBoostCurseType type,
                       const int missileId,
                       const float time,
                       const bool recalc = true);

    void addAura(const eAura& aura,
                 const bool self,
                 const bool recalc = true);
    void removeAllAuras(const bool recalc = true);
    bool isAuraSource() const;
    float maxAuraRange() const;
    bool addAurasTo(eServerUnit& target) const;
    const std::set<uint32_t>& auraIds() const { return mAuraIds; }

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

    void killed(const eServerUnit& killed);
    void dieAndCast(const ePointF& from);

    void die(eExplodeType type = eExplodeType::none);
    void respawn();

    void recalculateStats();
    void recalculateAuras();

    int addSkill();

    std::vector<int> readySkills() const;

    const std::vector<int>& followers() const
    { return mFollowers; }
    std::vector<int>& followers()
    { return mFollowers; }
    std::vector<int> followers(const int charDataId) const;
    int countFollowers(const int charDataId) const;

    void setMoving(const bool m) { mMoving = m; }
    bool moving() const;

    eUnitType unitType() const { return mType; }
    void setUnitType(const eUnitType type) { mType = type; }

    void coldFor(const float frameLen);
    void freezeFor(const float frameLen);
private:
    const eCharData& mData;
    eServerArea& mArea;
    const bool mSlayer;
    const int mUnitTypeId;

    bool mDead = false;

    bool mMoving = false;
    bool mAggressive = false;
    std::shared_ptr<eComplexAction> mAction;
    eMovementHandler mHandler;

    eStats mStats;
    eAttributes mAttributes;
    eEquipment mEquipment;

    eWeaponChoice mLastUsedWeapon = eWeaponChoice::left;

    float mColdLength = 0.f;
    float mFreezeLength = 0.f;

    const float mDealsDamagePeriod = 50.f;
    float mDealsDamageCounter = 0.f;

    std::vector<eTimedBoost> mBoosts;
    std::set<uint8_t> mAuras;
    std::set<uint32_t> mAuraIds;

    float mPoisonHitCounter = 0.f;
    std::vector<ePoisonDamage> mPoison;
    std::map<ePotionType, ePotionHealing> mPotions;
    std::vector<int> mFollowers;

    eUnitType mType = eUnitType::normal;
};

#endif // ESERVERUNIT_H

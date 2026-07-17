#ifndef ESERVERUNIT_H
#define ESERVERUNIT_H

#include "actions/ecomplexaction.h"

#include "eservermovementhandler.h"

#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/edamage.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/epotiontype.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eskillchoice.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eweaponchoice.h>
#include <eSlayerHelpers/eweapontype.h>
#include <eSlayerHelpers/efollowers.h>

#include <memory>
#include <atomic>

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
    float fHealthPerFrame = 0.f;
    float fManaPerFrame = 0.f;
    float fStaminaPerFrame = 0.f;
    float fFrameLength = 0.f;
};

enum eUnitType {
    slayer,
    body,
    mercenary,
    summoned,
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
    static std::atomic<uint32_t> sNextCharId;

    eServerUnit(const eUnitType type,
                const eCharData& data,
                const int unitTypeId,
                eServerArea& area);

    bool isSlayer() const { return mType == eUnitType::slayer; }

    bool aggressive() const { return mAggressive; }

    int level() const { return mAttributes.fLevel; }

    bool hitData(const eSkillStats& skill,
                 const eWeaponChoice wchoice,
                 eHitData& data) const;

    float defense() const;

    float blockChance() const;

    bool isRunning() const;

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
    eStats& stats() { return mStats; }

    void setEquipment(const eEquipment& eq,
                      const bool recalc = true);
    eEquipment& equipment() { return mEquipment; }

    void setAttributes(const eAttributes& attrs,
                       const bool recalc = true);
    eAttributes& attributes() { return mAttributes; }
    bool attributesChanged() const { return mAttributesChanged; }
    void setAttributesChanged(const bool c) { mAttributesChanged = c; }

    void setSkillLevels(const eSkillLevels& skillLevels,
                        const bool recalc = true);
    void consumePotion(const eItem& item);
    eItem takePotion(const uint32_t itemId);

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

    float manaBurn(const int schoice,
                   const eWeaponChoice wchoice) const;
    static float manaBurn(const eSkillStats& stats,
                          const eWeaponChoice wchoice);

    float spectralHit(const int schoice,
                      const eWeaponChoice wchoice) const;
    static float spectralHit(const eSkillStats& stats,
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
    void restoreStamina(const float by);
    void healAll();

    bool consumeMana(const float mana);
    eDamage attackDamage(const int schoice,
                         const eWeaponChoice wchoice);
    static eDamage attackDamage(const eSkillStats& stats,
                                const eWeaponChoice wchoice);

    float mana() const { return mStats.fManaF; }
    float stamina() const { return mStats.fStaminaF; }
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
    void addBoost(const std::vector<eModifier>& mods,
                  const eBoostCurseType type,
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

    eServerMovementHandler& movementHandler()
    { return mHandler; }
    const eServerMovementHandler& movementHandler() const
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

    const eFollowersBase& followers() const
    { return mFollowers; }
    eFollowersBase& followers()
    { return mFollowers; }
    eFollowersBase followers(const int unitInfoId) const;
    int countFollowers(const int unitInfoId) const;

    void setMoving(const bool m) { mMoving = m; }
    bool moving() const;

    eUnitType unitType() const { return mType; }

    void coldFor(const float frameLen);
    void freezeFor(const float frameLen);

    uint16_t requestUpdate(const uint32_t clientId);
    void update(const eUnitData::eShift shift);
    void updateAll();

    void setPosition(const ePointF& pos);
    void setMapId(const uint8_t mapId);
    void setAreaId(const uint8_t areaId);

    void setAnim(const uint8_t anim);
    void setAnimId(const eAnimId& animId);
    void incAnimId(const int by);
    void setAnimSpeed(const float animSpeed);
    void setBlockingActionTime(const float time);

    void setAngle(const float angle);

    void setHealth(const uint16_t health);
    void setMaxHealth(const uint16_t maxHealth);

    void setState(const uint8_t state);
    void setBoosts(const std::set<uint8_t>& boosts);

    void setTeamId(const eTeamId teamId);

    void setCold(const bool c);
    void setFrozen(const bool f);
    void setPoisoned(const bool p);
    void setStaminaPotion(const bool p);

    void applyBoostsTmp();

    void setArea(eServerArea& area);
    void setMercType(const int mercType);
private:
    using eUnitData::setUpdate;
    using eUnitData::fUpdate;

    using eUnitData::setPosition;
    using eUnitData::setMapId;
    using eUnitData::setAreaId;

    using eUnitData::setAnim;
    using eUnitData::setAnimId;
    using eUnitData::incAnimId;
    using eUnitData::setAnimSpeed;
    using eUnitData::setBlockingActionTime;

    using eUnitData::setAngle;

    using eUnitData::setHealth;
    using eUnitData::setMaxHealth;

    using eUnitData::setState;
    using eUnitData::setBoosts;

    using eUnitData::setTeamId;

    using eUnitData::setCold;
    using eUnitData::setFrozen;
    using eUnitData::setPoisoned;
    using eUnitData::setStaminaPotion;

    void removeBoostDataTmp(const uint8_t id);
    void addBoostDataTmp(const uint8_t id);

    std::set<uint8_t> fBoostsTmp;

    const eCharData& mData;
    const eUnitType mType;
    eServerArea* mArea = nullptr;

    bool mDead = false;

    bool mMoving = false;
    bool mAggressive = false;
    std::shared_ptr<eComplexAction> mAction;
    eServerMovementHandler mHandler;

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
    std::map<uint8_t, std::map<uint8_t, ePotionHealing>> mPotions;
    eFollowersBase mFollowers;

    int mMercType = -1;

    std::map<uint32_t, uint16_t> mUpdateMap;
    bool mAttributesChanged = false;
};

#endif // ESERVERUNIT_H

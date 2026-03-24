#ifndef ESERVERUNIT_H
#define ESERVERUNIT_H

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/eskills.h>

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
    float attackRating() const { return mAttackRating; }
    float defense() const;
    float blockChance() const;
    void setBlockChance(const float c) { mBlockChance = c; }
    float fasterHitRecovery() const { return mFasterHitRecovery; }
    void setFasterHitRecovery(const float fhr) { mFasterHitRecovery = fhr; }
    float fasterBlockRate() const { return mFasterBlockRate; }
    void setFasterBlockRate(const float fbr) { mFasterBlockRate = fbr; }
    float weaponRange() const { return mWeaponRange; }
    void setWeaponRange(const float r) { mWeaponRange = r; }
    float itemsAttackSpeed() const { return mItemsAttackSpeed; }
    void setItemsAttackSpeed(const float s) { mItemsAttackSpeed = s; }
    float itemsCastRate() const { return mItemsCastRate; }
    void setItemsCastRate(const float r) { mItemsCastRate = r; }

    static float sHitChance(const eServerUnit& hit,
                            const eServerUnit& by);

    bool getHit(const eHitData& data);

    void increment(const float by);
    bool skillReady(const int skillId) const;
    void useSkill(const int skillId);

    void setAction(const std::shared_ptr<eComplexAction>& a);
    void setChildAction(const std::shared_ptr<eUnitAction>& a);
    const std::shared_ptr<eComplexAction>&
    action() const { return mAction; }

    eMovementHandler& movementHandler()
    { return mHandler; }
    const eMovementHandler& movementHandler() const
    { return mHandler; }

    const eCharData& data() const { return mData; }

    float fHealthF = 100.f;
private:
    const eCharData& mData;
    bool mAggressive = false;
    std::shared_ptr<eComplexAction> mAction;
    eMovementHandler mHandler;

    float mLevel = 1.f;
    float mAttackRating = 100.f;
    float mDefense = 100.f;
    float mBlockChance = 0.f;
    float mFasterHitRecovery = 0.f;
    float mFasterBlockRate = 0.f;
    float mItemsAttackSpeed = 0.f;
    float mItemsCastRate = 0.f;
    float mWeaponRange = 0.f;

    std::map<int, float> mCooldowns;
};

#endif // ESERVERUNIT_H

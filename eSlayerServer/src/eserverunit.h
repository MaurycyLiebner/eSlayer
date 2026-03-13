#ifndef ESERVERUNIT_H
#define ESERVERUNIT_H

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/emovementhandler.h>

#include <memory>

class eUnitAction;
class eCharData;

class eServerUnit : public eUnitData {
public:
    static int sNextCharId;

    eServerUnit(const eCharData& data);

    bool aggressive() const { return mAggressive; }

    double level() const { return mLevel; }
    double attackRating() const { return mAttackRating; }
    double defense() const { return mDefense; }
    double blockChance() const { return mBlockChance; }
    void setBlockChance(const double c) { mBlockChance = c; }

    static double sHitChance(const eServerUnit& hit,
                             const eServerUnit& by);

    void increment(const double by);

    void setAction(const std::shared_ptr<eUnitAction>& a);
    const std::shared_ptr<eUnitAction>&
    action() const { return mAction; }

    eMovementHandler& movementHandler()
    { return mHandler; }
    const eMovementHandler& movementHandler() const
    { return mHandler; }

    const eCharData& data() const { return mData; }
private:
    const eCharData& mData;
    bool mAggressive = false;
    std::shared_ptr<eUnitAction> mAction;
    eMovementHandler mHandler;

    double mLevel = 1.;
    double mAttackRating = 100.;
    double mDefense = 100.;
    double mBlockChance = 0.;
};

#endif // ESERVERUNIT_H

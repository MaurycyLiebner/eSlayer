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
};

#endif // ESERVERUNIT_H

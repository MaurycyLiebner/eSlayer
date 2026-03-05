#ifndef ESERVERUNIT_H
#define ESERVERUNIT_H

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/emovementhandler.h>

#include <memory>

class eUnitAction;

class eServerUnit : public eUnitData {
public:
    static int sNextCharId;

    void increment(const double by);

    void setAction(const std::shared_ptr<eUnitAction>& a)
    { mAction = a; }

    eMovementHandler& movementHandler()
    { return mHandler; }
    const eMovementHandler& movementHandler() const
    { return mHandler; }
private:
    std::shared_ptr<eUnitAction> mAction;
    eMovementHandler mHandler;
};

#endif // ESERVERUNIT_H

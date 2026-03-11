#ifndef EATTACKACTION_H
#define EATTACKACTION_H

#include "eunitaction.h"

#include <functional>

using eAction = std::function<void()>;

class eAttackAction : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const double by) override;

    void setDuration(const double d);
    void setAction(const double time, const eAction& a);
private:
    double mDuration;
    double mActionTime;

    eAction mAction;
};

#endif // EATTACKACTION_H

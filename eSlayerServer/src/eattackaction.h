#ifndef EATTACKACTION_H
#define EATTACKACTION_H

#include "eunitaction.h"

class eAttackAction : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const double by) override;
private:
    double mAttackTime = 20.;
};

#endif // EATTACKACTION_H

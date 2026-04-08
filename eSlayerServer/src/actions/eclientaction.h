#ifndef ECLIENTACTION_H
#define ECLIENTACTION_H

#include "ecomplexaction.h"

#include <eSlayerHelpers/eattackdata.h>

class eClientAction : public eComplexAction {
public:
    using eComplexAction::eComplexAction;

    void increment(const float by) override;
    void decide() override;

    void attack(const eAttackData& target);
private:
    eAttackData mAttackTarget;
    bool mStopPlanned = false;
};

#endif // ECLIENTACTION_H

#ifndef EUNITBASEACTION_H
#define EUNITBASEACTION_H

#include "ecomplexaction.h"

#include <eSlayerHelpers/epoint.h>

class eUnitBaseAction : public eComplexAction {
public:
    using eComplexAction::eComplexAction;

    void decide() override;
private:
    void attack(const eServerUnit& u);
};

#endif // EUNITBASEACTION_H

#ifndef EUNITBASEACTION_H
#define EUNITBASEACTION_H

#include "ecomplexaction.h"

#include <eSlayerHelpers/epoint.h>

class eUnitBaseAction : public eComplexAction {
public:
    using eComplexAction::eComplexAction;

    void decide() override;
};

#endif // EUNITBASEACTION_H

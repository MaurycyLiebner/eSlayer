#ifndef ENPCACTION_H
#define ENPCACTION_H

#include "eunitbaseaction.h"

class eNPCAction : public eUnitBaseAction {
public:
    using eUnitBaseAction::eUnitBaseAction;

    void increment(const float by) override;
};

#endif // ENPCACTION_H

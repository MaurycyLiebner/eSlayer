#ifndef EUNITBASEACTION_H
#define EUNITBASEACTION_H

#include "eunitaction.h"

class eUnitBaseAction : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const double by) override;
};

#endif // EUNITBASEACTION_H

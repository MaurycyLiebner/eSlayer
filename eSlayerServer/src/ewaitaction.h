#ifndef EWAITACTION_H
#define EWAITACTION_H

#include "eunitaction.h"

class eWaitAction : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const double by) override;

    void setRemTime(const double t);
private:
    double mRemTime = 0.;
};

#endif // EWAITACTION_H

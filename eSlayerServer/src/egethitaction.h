#ifndef EGETHITACTION_H
#define EGETHITACTION_H

#include "eunitaction.h"

#include <memory>

class eGetHitAction : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    static std::shared_ptr<eGetHitAction>
    sCreate(eServerUnit& unit, eServerArea& area);

    void setRemTime(const double t);

    void increment(const double by) override;
private:
    double mRemTime = 0.;
};

#endif // EGETHITACTION_H

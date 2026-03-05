#ifndef EMOVETOENEMYACTION_H
#define EMOVETOENEMYACTION_H

#include "eunitaction.h"

#include <eSlayerHelpers/epoint.h>

class eMoveToEnemyAction : public eUnitAction {
public:
    using eUnitAction::eUnitAction;

    void increment(const double by) override;
private:
    void setTarget(const eServerUnit& u);
    int mTargetId = -1;
    ePointF mTargetPos{0., 0.};
};

#endif // EMOVETOENEMYACTION_H

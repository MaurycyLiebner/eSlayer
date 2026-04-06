#ifndef EMOVETOTARGET_H
#define EMOVETOTARGET_H

#include "eunitaction.h"

#include <eSlayerHelpers/epoint.h>

class eMoveToTarget : public eUnitAction {
public:
    eMoveToTarget(eServerUnit& unit,
                  eServerArea& area,
                  const int runAnimId,
                  const int walkAnimId,
                  const int walkReadyAnimId);

    void increment(const float by) override;

    void setArriveDist(const float dist) { mArriveDist = dist; }
    void setTarget(const eServerUnit& u);
protected:
    int mTargetId = -1;
    ePointF mTargetPos{0.f, 0.f};
private:
    const int mRunAnimId;
    const int mWalkAnimId;
    const int mWalkReadyAnimId;

    float mArriveDist = 0.f;
};

#endif // EMOVETOTARGET_H

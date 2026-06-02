#ifndef EMOVETOTARGET_H
#define EMOVETOTARGET_H

#include "actions/eunitaction.h"

#include <eSlayerHelpers/epoint.h>

struct eUnitTarget {
    eUnitTarget(const uint32_t id,
                const ePointF& pos) :
        fId(id), fPos(pos) {}

    uint32_t fId;
    ePointF fPos;
};

class eMoveToTarget : public eUnitAction {
public:
    eMoveToTarget(eServerUnit& unit,
                  eServerArea& area,
                  const int runAnimId,
                  const int walkAnimId,
                  const int walkReadyAnimId);

    void increment(const float by) override;

    void setArriveDist(const float dist) { mArriveDist = dist; }
    bool setTarget(const std::vector<eUnitTarget>& targets,
                   const bool foundOnly);
protected:
    std::vector<eUnitTarget> mTargets;
    bool mFoundOnly = false;
private:
    const int mRunAnimId;
    const int mWalkAnimId;
    const int mWalkReadyAnimId;

    float mArriveDist = 0.f;
};

#endif // EMOVETOTARGET_H

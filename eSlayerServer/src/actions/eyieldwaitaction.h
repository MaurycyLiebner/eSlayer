#ifndef EYIELDWAITACTION_H
#define EYIELDWAITACTION_H

#include "eunitactionbase.h"

#include <memory>

class eYieldWaitAction : public eUnitActionBase {
public:
    using eUnitActionBase::eUnitActionBase;

    static std::shared_ptr<eYieldWaitAction>
    sCreateStand(eServerUnit& unit, eServerArea& area,
                 const int standId, const int standReadyId,
                 const int walkId, const int walkReadyId,
                 const float time);

    void increment(const float by) override;
private:
    int mStandAnimId = -1;
    int mStandReadyAnimId = -1;
    int mWalkAnimId = -1;
    int mWalkReadyAnimId = -1;
    bool mYielding = false;
};

#endif // EYIELDWAITACTION_H

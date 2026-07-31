#ifndef ENPCACTION_H
#define ENPCACTION_H

#include "ecomplexaction.h"

enum class eNPCStage {
    none,
    wounded,
    standCured,
    leave
};

class eNPCAction : public eComplexAction {
public:
    eNPCAction(eServerUnit& unit,
               eServerArea& area);

    void increment(const float by) override;
    void decide() override;

    void layWounded();
    void cure();
private:
    void wait(const float time);
    void walkAround(const float time);
    void goBack();
    void leave();

    eNPCStage mStage = eNPCStage::none;
    bool mInterruptable = false;
    ePointF mMainPos;
    int mWalkAnimId = -1;
    int mStandAnimId = -1;
    int mWoundedAnimId = -1;

    uint32_t mPortalId = 0;
    ePointF mPortalPos;
};

#endif // ENPCACTION_H

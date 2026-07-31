#ifndef ENPCACTION_H
#define ENPCACTION_H

#include "ecomplexaction.h"

class eNPCAction : public eComplexAction {
public:
    eNPCAction(eServerUnit& unit,
               eServerArea& area);

    void increment(const float by) override;
    void decide() override;
private:
    void wait(const float time);
    void walkAround(const float time);
    void goBack();

    bool mMoving = false;
    ePointF mMainPos;
    int mWalkAnimId = -1;
    int mStandAnimId = -1;
};

#endif // ENPCACTION_H

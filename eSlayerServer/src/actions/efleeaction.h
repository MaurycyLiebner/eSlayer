#ifndef EFLEEACTION_H
#define EFLEEACTION_H

#include "../emovetotarget.h"

class eFleeAction : public eMoveToTarget {
public:
    eFleeAction(eServerUnit& unit,
                const ePointF& from,
                eServerArea& area,
                const int runAnimId,
                const int walkAnimId,
                const int walkReadyAnimId,
                const float maxDist = 5.f);

    void increment(const float by);
};

#endif // EFLEEACTION_H

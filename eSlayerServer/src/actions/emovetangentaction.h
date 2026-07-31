#ifndef EMOVETANGENTACTION_H
#define EMOVETANGENTACTION_H

#include "emovetotarget.h"

class eMoveTangentAction :
    public eMoveToTarget {
public:
    eMoveTangentAction(
        eServerUnit& unit,
        const ePointF& from,
        eServerArea& area,
        const int runAnimId,
        const int walkAnimId,
        const int walkReadyAnimId,
        const float maxDist = 5.f);
};

#endif // EMOVETANGENTACTION_H

#ifndef EWAITACTION_H
#define EWAITACTION_H

#include "eunitactionbase.h"

#include <memory>

class eWaitAction : public eUnitActionBase {
public:
    using eUnitActionBase::eUnitActionBase;

    static std::shared_ptr<eWaitAction>
    sCreateStand(eServerUnit& unit, eServerArea& area,
                 const int standId, const int standReadyId,
                 const float time);
    static std::shared_ptr<eWaitAction>
    sCreateDeath(eServerUnit& unit, eServerArea& area);
    static std::shared_ptr<eWaitAction>
    sCreateBody(eServerUnit& unit, eServerArea& area);
    static std::shared_ptr<eWaitAction>
    sCreateExplode(eServerUnit& unit, eServerArea& area);
    static std::shared_ptr<eWaitAction>
    sCreateExplodeBody(eServerUnit& unit, eServerArea& area);
    static std::shared_ptr<eWaitAction> sCreate(
        eServerUnit& unit, eServerArea& area,
        const int anim, const bool blocking);
};

#endif // EWAITACTION_H

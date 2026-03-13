#ifndef EHITRECOVERYACTION_H
#define EHITRECOVERYACTION_H

#include "eunitactionbase.h"

#include <memory>

class eHitRecoveryAction : public eUnitActionBase {
public:
    using eUnitActionBase::eUnitActionBase;

    static std::shared_ptr<eHitRecoveryAction>
    sCreate(eServerUnit& unit, eServerArea& area);
};

#endif // EHITRECOVERYACTION_H

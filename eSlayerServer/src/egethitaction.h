#ifndef EGETHITACTION_H
#define EGETHITACTION_H

#include "eunitactionbase.h"

#include <memory>

class eGetHitAction : public eUnitActionBase {
public:
    using eUnitActionBase::eUnitActionBase;

    static std::shared_ptr<eGetHitAction>
    sCreate(eServerUnit& unit, eServerArea& area);
};

#endif // EGETHITACTION_H

#ifndef EBLOCKACTION_H
#define EBLOCKACTION_H

#include "eunitactionbase.h"

#include <memory>

class eBlockAction : public eUnitActionBase {
public:
    using eUnitActionBase::eUnitActionBase;

    static std::shared_ptr<eBlockAction>
    sCreate(eServerUnit& unit, eServerArea& area);
};

#endif // EBLOCKACTION_H

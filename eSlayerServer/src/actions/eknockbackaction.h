#ifndef EKNOCKBACKACTION_H
#define EKNOCKBACKACTION_H

#include "eunitactionbase.h"

#include <eSlayerHelpers/evec2.h>

#include <memory>

class eKnockbackAction : public eUnitActionBase {
public:
    using eUnitActionBase::eUnitActionBase;

    static std::shared_ptr<eKnockbackAction>
    sCreate(eServerUnit& unit, eServerArea& area,
            const eVec2f& dir, const float dist);

    void increment(const float by);
private:
    void setupKnockback(eVec2f dir, const float dist);

    eVec2f mDir;
    float mRemDist = 0.f;
};

#endif // EKNOCKBACKACTION_H

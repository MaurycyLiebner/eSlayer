#include "eunitaction.h"

#include "ecomplexaction.h"

eUnitAction::eUnitAction(eServerUnit& unit,
                         eServerArea& area) :
    mUnit(unit), mArea(area) {}

void eUnitAction::setParent(eComplexAction* const c) {
    mParent = c;
}

void eUnitAction::finishAction() {
    if(mParent) {
        const auto tmp = mParent;
        mParent = nullptr;
        tmp->setChild(nullptr);
    }
}

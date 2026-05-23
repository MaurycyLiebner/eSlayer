#include "eexplodeaction.h"

#include "ewaitaction.h"

eExplodeAction::eExplodeAction(
    const eExplodeType type,
    eServerUnit& unit,
    eServerArea& area) :
    eComplexAction(unit, area),
    mType(type) {}

void eExplodeAction::decide() {
    switch(mStage) {
    case eExplodeStage::none: {
        mStage = eExplodeStage::explode;
        const auto death = eWaitAction::sCreateExplode(
            mType, mUnit, mArea);
        if(death) setChild(death);
        else decide();
    } break;
    case eExplodeStage::explode: {
        finishAction();
    } break;
    }
}

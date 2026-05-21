#include "eexplodeaction.h"

#include "ewaitaction.h"

void eExplodeAction::decide() {
    switch(mStage) {
    case eExplodeStage::none: {
        mStage = eExplodeStage::explode;
        const auto death = eWaitAction::sCreateExplode(mUnit, mArea);
        if(death) setChild(death);
        else decide();
    } break;
    case eExplodeStage::explode: {
        mStage = eExplodeStage::body;
        const auto body = eWaitAction::sCreateExplodeBody(mUnit, mArea);
        if(body) setChild(body);
        else decide();
    } break;
    case eExplodeStage::body: {
        finishAction();
    } break;
    }
}

#include "edieaction.h"

#include "ewaitaction.h"

#include <eSlayerHelpers/echardata.h>

void eDieAction::decide() {
    switch(mStage) {
    case eDieStage::none: {
        mStage = eDieStage::collapse;
        const auto death = eWaitAction::sCreateDeath(mUnit, mArea);
        if(death) setChild(death);
        else decide();
    } break;
    case eDieStage::collapse: {
        mStage = eDieStage::body;
        const auto body = eWaitAction::sCreateBody(mUnit, mArea);
        if(body) setChild(body);
        else decide();
    } break;
    case eDieStage::body: {
        finishAction();
    } break;
    }
}

#include "egethitaction.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eGetHitAction>
eGetHitAction::sCreate(
    eServerUnit& unit, eServerArea& area) {
    const auto& data = unit.data();
    const int aId = data.animId("getHit");
    if(aId != -1) {
        const auto getHit = std::make_shared<eGetHitAction>(unit, area);
        const int frames = data.animFrames(aId);
        getHit->setRemTime(frames);
        unit.fAnim = aId;
        unit.fAnimId++;
        return getHit;
    } else {
        return nullptr;
    }
}

void eGetHitAction::setRemTime(const double t) {
    mRemTime = t;
}

void eGetHitAction::increment(const double by) {
    mRemTime -= by;
    if(mRemTime <= 0) finishAction();
}

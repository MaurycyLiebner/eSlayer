#include "edieaction.h"

#include "eserverunit.h"
#include "ewaitaction.h"

#include <eSlayerHelpers/echardata.h>

void eDieAction::decide() {
    const auto& data = mUnit.data();
    const int deathId = data.animId("death");
    const auto wait = std::make_shared<eWaitAction>(mUnit, mArea);
    if(mUnit.fAnim == deathId) {
        const int bodyId = data.animId("body");
        mUnit.fAnim = bodyId;
        wait->setRemTime(std::numeric_limits<double>::max());
    } else {
        mUnit.fAnim = deathId;
        wait->setRemTime(data.animFrames(deathId));
    }
    mUnit.fAnimId++;
    setChild(wait);
}

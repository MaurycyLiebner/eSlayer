#include "eknockbackaction.h"

#include "../eserverunit.h"
#include "ehitrecoveryaction.h"
#include "../eserverarea.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eKnockbackAction>
eKnockbackAction::sCreate(
    eServerUnit& unit, eServerArea& area,
    const eVec2f& dir, const float dist) {
    const auto& data = unit.data();
    const int anim = data.hitRecoveryAnimId();
    if(anim != -1) {
        const int frames = eHitRecoveryAction::sRecoveryFrames(unit);
        const auto a = std::make_shared<eKnockbackAction>(unit, area);
        a->setup(anim, frames, true, nullptr);
        a->setupKnockback(dir, dist);
        return a;
    } else {
        return nullptr;
    }
}

void eKnockbackAction::increment(const float by) {
    if(mRemDist > 0.f) {
        const auto displ = mDir*(0.5f*by);
        mRemDist -= displ.length();
        const auto newPos = mUnit.fPos + displ;
        const bool w = mArea.walkable(newPos);
        if(w) mUnit.fPos = newPos;
    }
    eUnitActionBase::increment(by);
}

void eKnockbackAction::setupKnockback(eVec2f dir, const float dist) {
    mDir = dir;
    mDir.normalize();
    mRemDist = dist;
}

#include "eknockbackaction.h"

#include "../eserverunit.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eKnockbackAction>
eKnockbackAction::sCreate(
    eServerUnit& unit, eServerArea& area,
    const eVec2f& dir, const float dist) {
    const auto& data = unit.data();
    const int anim = data.animId("hitRecovery");
    if(anim != -1) {
        const float fhr = unit.fasterHitRecovery();
        const float efhr = std::floor(fhr*120.f/(fhr + 120.f));
        const int hitBase = data.animFrames(anim);
        const float animSpeed = 256.f;
        const int frames = int(std::ceil(256.f*hitBase/std::floor(animSpeed*(50.f + efhr)/100.f))) - 1;
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
        mUnit.fPos = mUnit.fPos + displ;
    }
    eUnitActionBase::increment(by);
}

void eKnockbackAction::setupKnockback(eVec2f dir, const float dist) {
    mDir = dir;
    mRemDist = dist;
}

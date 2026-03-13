#include "eunitactionbase.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

void eUnitActionBase::increment(const double by) {
    mRemTime -= by;
    mUnit.fActionTime = mRemTime;
    mActionTime -= by;
    if(mAction && mActionTime <= 0) {
        mAction();
        mAction = nullptr;
    }
    if(mRemTime <= 0) finishAction();
}

void eUnitActionBase::setDuration(const double d) {
    mRemTime = d;
    mUnit.fActionTime = d;
}

void eUnitActionBase::setAction(const double time, const eAction& a) {
    mActionTime = time;
    mAction = a;
}

void eUnitActionBase::setup(
    const int anim, const eAction& a) {
    const auto& data = mUnit.data();
    mUnit.fAnim = anim;
    mUnit.fAnimId++;
    const int frames = data.animFrames(anim);
    setDuration(frames);

    if(a) {
        const int frame = data.animActionFrame(anim);
        setAction(frame, a);
    }
}

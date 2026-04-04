#include "eunitactionbase.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

void eUnitActionBase::increment(const float by) {
    mRemTime -= by;
    mUnit.fActionTime = mRemTime;
    mActionTime -= by;
    if(mAction && mActionTime <= 0) {
        mAction();
        mAction = nullptr;
    }
    if(mRemTime <= 0) finishAction();
}

void eUnitActionBase::setDuration(const float d) {
    mRemTime = d;
    mUnit.fActionTime = d;
}

void eUnitActionBase::setAction(const float time, const eAction& a) {
    mActionTime = time;
    mAction = a;
}

void eUnitActionBase::setup(
    const int anim,
    int frames,
    const eAction& a) {
    const auto& data = mUnit.data();
    mUnit.fAnim = anim;
    mUnit.fAnimId += 5;
    const int baseFrames = data.animFrames(anim);
    if(frames == -1) frames = baseFrames;
    const float speed = frames == 0 ? 1.f : float(baseFrames)/frames;
    mUnit.fAnimSpeed = speed;
    setDuration(frames);

    if(a) {
        const int baseFrame = data.animActionFrame(anim);
        const float frame = baseFrame/speed;
        setAction(frame, a);
    }
}

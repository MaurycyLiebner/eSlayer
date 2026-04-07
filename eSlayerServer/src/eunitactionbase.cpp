#include "eunitactionbase.h"

#include "eserverunit.h"

#include <eSlayerHelpers/echardata.h>

void eUnitActionBase::increment(const float by) {
    mRemTime -= by;
    if(mBlockingAction) {
        mUnit.fBlockingActionTime = mRemTime;
    } else {
        mUnit.fBlockingActionTime = 0.f;
    }
    mActionTime -= by;
    if(mAction && mActionTime <= 0) {
        mAction();
        mAction = nullptr;
    }
    if(mRemTime <= 0) finishAction();
}

void eUnitActionBase::setDuration(const float d) {
    mRemTime = d;
    if(mBlockingAction) {
        mUnit.fBlockingActionTime = d;
    } else {
        mUnit.fBlockingActionTime = 0.f;
    }
}

void eUnitActionBase::setup(const int anim,
                            int frames,
                            bool blocking,
                            const eAction& a) {
    mBlockingAction = blocking;
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
        mActionTime = frame;
        mAction = a;
    }
}

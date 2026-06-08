#include "eunitactionbase.h"

#include "../eserverunit.h"

#include <eSlayerHelpers/echardata.h>

void eUnitActionBase::increment(const float by) {
    setDuration(mRemTime - by);

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
        mUnit.setBlockingActionTime(d);
    } else {
        mUnit.setBlockingActionTime(0.f);
    }
}

void eUnitActionBase::setup(const int anim,
                            int frames,
                            bool blocking,
                            const eAction& a) {
    mBlockingAction = blocking;
    const auto& data = mUnit.data();
    mUnit.setAnim(anim);
    mUnit.incAnimId(5);

    float speed;
    if(anim == sFleshExplAnim ||
       anim == sIceExplAnim) {
        speed = 1.f;
        setDuration(std::numeric_limits<float>::max());
    } else {
        const int baseFrames = data.animFrames(anim);
        if(frames == -1) frames = baseFrames;
        speed = frames == 0 ? 1.f : float(baseFrames)/frames;
        mUnit.setAnimSpeed(speed);
        setDuration(frames);
    }

    if(a) {
        const int baseFrame = data.animActionFrame(anim);
        const float frame = baseFrame/speed;
        mActionTime = frame;
        mAction = a;
    }
}

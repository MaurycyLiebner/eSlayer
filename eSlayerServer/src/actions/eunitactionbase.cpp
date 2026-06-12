#include "eunitactionbase.h"

#include "../eserverunit.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/emissilesinfo.h>
#include <eSlayerHelpers/especialanim.h>

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

    int baseFrames;
    if(eSpecialAnim::isSpecial(anim)) {
        baseFrames = eSpecialAnim::nFrames(anim);
    } else {
        baseFrames = data.animFrames(anim);
    }
    if(frames == -1) frames = baseFrames;
    const float speed = frames == 0 ? 1.f : float(baseFrames)/frames;
    setDuration(frames);
    mUnit.setAnimSpeed(speed);

    if(a) {
        const int baseFrame = data.animActionFrame(anim);
        const float frame = baseFrame/speed;
        mActionTime = frame;
        mAction = a;
    }
}

#include "ewalkaroundaction.h"

#include "../eserverunit.h"

std::shared_ptr<eWalkAroundAction>
eWalkAroundAction::sCreate(
    eServerUnit& unit,
    eServerArea& area,
    const int walkId,
    const int walkReadyId,
    const float time) {
    const auto result = std::make_shared<eWalkAroundAction>(unit, area);
    const bool a = unit.aggressive();
    const int anim = eMovementHandlerBase::sChooseAnim(
        walkId, walkReadyId, a);
    result->mCurrentAnim = anim;
    result->mWalkId = walkId;
    result->mWalkReadyId = walkReadyId;
    result->setup(anim, -1, false, nullptr);
    result->setDuration(time);
    return result;
}

void eWalkAroundAction::increment(const float by) {
    const bool a = mUnit.aggressive();
    const int anim = eMovementHandlerBase::sChooseAnim(
        mWalkId, mWalkReadyId, a);
    if(anim != mCurrentAnim) {
        mCurrentAnim = anim;
        mUnit.setAnim(anim);
        mUnit.incAnimId(5);
    }

    auto& h = mUnit.movementHandler();
    const float changePeriod = 40.f;
    mDirChangeCounter += by;
    if(mMoveDir.length() == 0.f ||
       mDirChangeCounter > changePeriod ||
       h.stuckTime() > 5.f) {
        mDirChangeCounter = 0.f;
        mMoveDir = eVec2f::random();
    }
    h.moveInDirection(mUnit.fPos + mMoveDir);

    eUnitActionBase::increment(by);
}

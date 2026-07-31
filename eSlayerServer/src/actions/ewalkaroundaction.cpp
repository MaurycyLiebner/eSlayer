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
    result->mCenterPos = unit.fPos;
    result->mCurrentAnim = anim;
    result->mWalkId = walkId;
    result->mWalkReadyId = walkReadyId;
    result->setup(anim, -1, false, nullptr);
    result->setDuration(time);
    return result;
}

void eWalkAroundAction::setMaxDist(
    const float maxDist) {
    mMaxDist = maxDist;
}

void eWalkAroundAction::setCenterPos(const ePointF& pos) {
    mCenterPos = pos;
}

bool pointsToward(const eVec2f& v, const eVec2f& direction) {
    return eVec2f::dot(v, direction) > 0.f;
}

void eWalkAroundAction::increment(const float by) {
    auto& h = mUnit.movementHandler();
    if(mUnit.immobilized()) {
        h.stopMoving();
        return finishAction();
    }
    const bool a = mUnit.aggressive();
    const int anim = eMovementHandlerBase::sChooseAnim(
        mWalkId, mWalkReadyId, a);
    if(anim != mCurrentAnim) {
        mCurrentAnim = anim;
        mUnit.setAnim(anim);
        mUnit.incAnimId(5);
    }

    const float changePeriod = 40.f;
    mDirChangeCounter += by;

    if(mMoveDir.length() == 0.f ||
       mDirChangeCounter > changePeriod ||
       h.stuckTime() > 5.f) {
        mDirChangeCounter = 0.f;
        mMoveDir = eVec2f::random();
    }

    if(mMaxDist) {
        const float maxDist = *mMaxDist;
        const float dist = ePointF::distance(
            mUnit.fPos, mCenterPos);
        if(dist > maxDist) {
            const auto homeDir = ePointF::vector(mCenterPos, mUnit.fPos);
            if(!pointsToward(mMoveDir, homeDir)) {
                mMoveDir = eVec2f{-mMoveDir.x, -mMoveDir.y};
            }
        }
    }

    h.moveInDirection(mUnit.fPos + mMoveDir);

    eUnitActionBase::increment(by);
}

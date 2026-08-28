#include "eyieldwaitaction.h"

#include "../eserverarea.h"
#include "../eserverunit.h"

#include <eSlayerHelpers/echardata.h>

std::shared_ptr<eYieldWaitAction>
eYieldWaitAction::sCreateStand(
    eServerUnit& unit, eServerArea& area,
    const int standId, const int standReadyId,
    const int walkId, const int walkReadyId,
    const float time) {
    auto& h = unit.movementHandler();
    h.stopMoving();
    const bool a = unit.aggressive();
    const int anim = eMovementHandlerBase::sChooseAnim(
        standId, standReadyId, a);
    if(anim == -1) return nullptr;
    const auto result = std::make_shared<eYieldWaitAction>(unit, area);
    result->mStandAnimId = standId;
    result->mStandReadyAnimId = standReadyId;
    result->mWalkAnimId = walkId;
    result->mWalkReadyAnimId = walkReadyId;
    result->setup(anim, -1, false, nullptr);
    result->setDuration(time);
    return result;
}

void eYieldWaitAction::increment(const float by) {
    // Check if a same-team unit is moving toward us
    bool shouldYield = false;
    eVec2f yieldDir{0.f, 0.f};

    const float detectRadius = 2.f;
    const auto iter = [&](const std::shared_ptr<eServerUnit>& other) {
        if(other->fCharId == mUnit.fCharId) return false;
        const eTeamId t1 = other->fTeamId;
        const eTeamId t2 = mUnit.fTeamId;
        if(eTeams::areEnemies(t1, t2)) return false;
        if(other->fHealth <= 0) return false;
        if(!other->moving()) return false;

        const eVec2f toUs = ePointF::vector(mUnit.fPos, other->fPos);
        const float dist = toUs.length();
        if(dist > detectRadius || dist < 0.001f) return false;

        // Check if the other unit is heading toward us
        // fAngle tracks the unit's movement direction
        const float angleRad = other->fAngle * M_PI / 180.f;
        const eVec2f otherDir{std::cos(angleRad), std::sin(angleRad)};
        auto toUsNorm = toUs;
        toUsNorm.normalize();
        const float dot = eVec2f::dot(toUsNorm, otherDir);
        // dot > 0 means heading generally toward us
        if(dot < 0.3f) return false;

        const float minDist = mUnit.fRadius + other->fRadius;
        if(dist > minDist * 2.f) return false;

        // Compute perpendicular direction to step aside
        eVec2f perp{-toUs.y, toUs.x};
        perp.normalize();
        yieldDir += perp;
        shouldYield = true;
        return false;
    };
    const bool immobilized = mUnit.totallyImmobilized();
    if(!immobilized) {
        mArea.iterateOverUnits(mUnit.fPos, detectRadius, iter);
    }

    auto& h = mUnit.movementHandler();
    if(shouldYield) {
        if(!mYielding) {
            mYielding = true;
            const bool a = mUnit.aggressive();
            const int walkAnim = eMovementHandlerBase::sChooseAnim(
                mWalkAnimId, mWalkReadyAnimId, a);
            if(walkAnim != -1) {
                mUnit.setAnim(walkAnim);
                mUnit.incAnimId(5);
            }
        }
        if(yieldDir.length() > 0.001f) {
            yieldDir.normalize();
        }
        const auto target = mUnit.fPos + yieldDir;
        h.moveInDirection(target);
        h.increment(by);
    } else {
        if(mYielding) {
            mYielding = false;
            h.stopMoving();
            const bool a = mUnit.aggressive();
            const int standAnim = eMovementHandlerBase::sChooseAnim(
                mStandAnimId, mStandReadyAnimId, a);
            if(standAnim != -1) {
                mUnit.setAnim(standAnim);
                mUnit.incAnimId(5);
            }
        }
    }

    eUnitActionBase::increment(by);
}

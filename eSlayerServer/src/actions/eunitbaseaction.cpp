#include "eunitbaseaction.h"

#include "../eserverarea.h"
#include "emovetoenemyaction.h"
#include "eyieldwaitaction.h"
#include "ewalkaroundaction.h"
#include "efleeaction.h"
#include "emovetangentaction.h"
#include "ewaitaction.h"

#include <eSlayerHelpers/echardata.h>
#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/evectorhelpers.h>

eUnitBaseAction::eUnitBaseAction(eServerUnit& unit,
                                 eServerArea& area) :
    eComplexAction(unit, area) {
    const auto& data = mUnit.data();
    mRunAnimId = data.animId("run");
    mWalkAnimId = data.animId("walk");
    mWalkReadyAnimId = data.animId("walkReady");
    mStandAnimId = data.animId("stand");
    mStandReadyAnimId = data.animId("standReady");

    mCanMove = mWalkAnimId != -1 ||
               mWalkReadyAnimId != -1 ||
               mRunAnimId != -1;

    const auto& info = eUnitsInfo::sUnits.get(
        unit.fUnitInfoId);

    mTanChance = info.fTanChance;
    mTanDistance = info.fTanDistance;

    mStandChance = info.fStandChance;
    mStandLength = info.fStandLength;
}

void eUnitBaseAction::increment(const float by) {
    if(mUnit.fHealth > 0 &&
       mUnit.fBlockingActionTime <= 0.f &&
       mStrategy == eUnitStrategy::attack) {
        checkForAttackIncrement(by);
    }
    eComplexAction::increment(by);
}

void eUnitBaseAction::planFlee(const eFlee& flee) {
    if(mUnit.fBlockingActionTime > 0.f) {
        mFleeFrom = flee;
    } else {
        eUnitBaseAction::flee(flee);
    }
}

bool eUnitBaseAction::checkForAttackIncrement(const float by) {
    if(mAttacking) return true;
    mAttackCounter += by;
    if(mAttackCounter >= sAttackCounterMax) {
        mAttackCounter = 0.f;
        return lookForAttackTarget();
    }
    return false;
}

void eUnitBaseAction::decide() {
    if(mUnit.fHealth <= 0) {
        mArea.planRemoveUnit(mUnit.fCharId);
        return;
    }
    const bool immobilized = mUnit.immobilized();
    if(!immobilized && mFleeFrom) {
        const bool r = flee(*mFleeFrom);
        mFleeFrom = std::nullopt;
        if(r) return;
    }
    if(!immobilized && mAttacking && mTanDistance > 0.f) {
        if(eRand::randChance(mTanChance)) {
            const auto u = mArea.unit(mAttacking);
            if(u) {
                mAttacking = 0;
                const auto move = std::make_shared<eMoveTangentAction>(
                    mUnit, u->fPos, mArea, mRunAnimId,
                    mWalkAnimId, mWalkReadyAnimId,
                    mTanDistance);
                setChild(move);
                setStrategy(eUnitStrategy::move);
                return;
            }
        }
    }
    if(mAttacking && mStandLength > 0 && eRand::randChance(mStandChance)) {
        const int standAnim = eMovementHandlerBase::sChooseAnim(
            mStandAnimId, mStandReadyAnimId, true);
        if(standAnim >= 0) {
            const auto stand = std::make_shared<eUnitActionBase>(
                mUnit, mArea);
            const auto& data = mUnit.data();
            const int frames = data.animFrames(standAnim);
            stand->setup(standAnim, frames, true, nullptr);
            stand->setDuration(mStandLength*frames);
            setChild(stand);
            mAttacking = 0;
            return;
        }
    }
    setStrategy(eUnitStrategy::attack);
    mAttacking = 0;
    mAttackCounter = 0.f;
    if(mStrategy == eUnitStrategy::attack) {
        {
            const bool r = lookForAttackTarget();
            if(r) return;
        }
        if(!immobilized) {
            const bool r = moveToEnemy(mAttackDist);
            if(r) return;
        }
    }
    if(!immobilized && eRand::rand() % 2) {
        walkAround(200.f);
    } else {
        wait(100.f);
    }
}

bool eUnitBaseAction::moveToEnemy(const float maxDist) {
    if(!mCanMove) return false;
    const auto move = std::make_shared<eMoveToEnemyAction>(
        mUnit, mArea, mRunAnimId,
        mWalkAnimId, mWalkReadyAnimId,
        maxDist);
    setChild(move);
    const bool r = move->findNewTarget();
    return r;
}

bool eUnitBaseAction::flee(const eFlee& flee) {
    if(!mCanMove) return false;
    if(flee.fDist <= 0.f) return false;
    const auto move = std::make_shared<eFleeAction>(
        mUnit, flee.fFrom, mArea, mRunAnimId,
        mWalkAnimId, mWalkReadyAnimId,
        flee.fDist);
    setChild(move);
    return true;
}

void eUnitBaseAction::wait(const float time) {
    if(mCanMove) {
        const auto wait = eYieldWaitAction::sCreateStand(
            mUnit, mArea, mStandAnimId, mStandReadyAnimId,
            mWalkAnimId, mWalkReadyAnimId, time);
        setChild(wait);
    } else {
        const auto wait = eWaitAction::sCreateStand(
            mUnit, mArea, mStandAnimId, mStandReadyAnimId,
            time);
        setChild(wait);
    }
}

void eUnitBaseAction::walkAround(const float time) {
    if(!mCanMove) return;
    const auto walkAround = eWalkAroundAction::sCreate(
        mUnit, mArea, mWalkAnimId, mWalkReadyAnimId, time);
    setChild(walkAround);
}

bool eUnitBaseAction::lookForAttackTarget() {
    mAttacking = 0;
    int maxRangeSchoice;
    const auto& stats = mUnit.stats();
    const float maxRange = stats.maxRangeSkill(
        maxRangeSchoice, 1.f, mUnit.fRadius);
    auto readySkills = mUnit.readySkills();
    std::vector<int> raiseSkills;
    for(const auto skillId : readySkills) {
        const auto& skill = eSkills::sSkills.get(skillId);
        const bool r = skill.fType == eSkillType::raise;
        if(r) raiseSkills.emplace_back(skillId);
    }
    if(!raiseSkills.empty()) {
        bool raising = false;
        const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
            const bool c = u->isCorpse();
            if(!c) return false;
            bool canRaise = false;
            int raiseSkillId = 0;
            for(const auto skillId : raiseSkills) {
                const auto& skill = eSkills::sSkills.get(skillId);
                const int infoId = u->fUnitInfoId;
                const bool r = eVectorHelpers::contains(
                    skill.fUnits, infoId);
                if(!r) continue;
                canRaise = true;
                raiseSkillId = skillId;
                break;
            }
            if(!canRaise) return false;
            if(u->fHealth <= 0) return false;
            const bool obstacle = mArea.obstacle(mUnit.fPos, u->fPos);
            if(obstacle) return false;
            const auto schoice = stats.schoiceForSkill(raiseSkillId);
            if(schoice >= 0) {
                const auto charId = u->fCharId;
                const eAttackData data(charId, schoice);
                const auto r = eComplexAction::attack(data);
                if(r == eAttackResult::attacked) {
                    raising = true;
                    return true;
                }
            }
            return false;
        };
        mArea.iterateOverUnitsClamped(mUnit.fPos, maxRange, iter);
        if(raising) return true;
    }
    const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
        if(u->fHealth <= 0) return false;
        const eTeamId t1 = u->fTeamId;
        const eTeamId t2 = mUnit.fTeamId;
        if(!eTeams::areEnemies(t1, t2)) return false;
        const bool obstacle = mArea.obstacle(mUnit.fPos, u->fPos);
        if(obstacle) return false;
        const float dist = ePointF::distance(mUnit.fPos, u->fPos);
        int schoice;
        const bool r = stats.attackRangeSkill(
            dist, schoice,
            u->fRadius, mUnit.fRadius,
            readySkills);
        if(r) {
            const auto charId = u->fCharId;
            const eAttackData data(charId, schoice);
            const auto r = eComplexAction::attack(data);
            if(r == eAttackResult::attacked) {
                mAttacking = u->fCharId;
                return true;
            }
        }
        return false;
    };
    mArea.iterateOverUnitsClamped(mUnit.fPos, maxRange, iter);
    return mAttacking > 0;
}

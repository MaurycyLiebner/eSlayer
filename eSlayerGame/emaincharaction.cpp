#include "emaincharaction.h"

#include "textures/echarstextures.h"
#include "textures/echartextures.h"
#include "units/eunit.h"

#include "widgets/gameScreen/einventorywidget.h"
#include "widgets/gameScreen/eitemdragwidget.h"

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerServer/eserver.h>

void eMainCharAction::initialize(const std::shared_ptr<eServer>& s,
                                 SDL_Renderer* const r,
                                 const eWalkable& w,
                                 const eOtherIterator& iter,
                                 const int clientId,
                                 const int teamId) {
    mClientId = clientId;
    mServer = s;
    mMovementHandler.intialize(w, iter, clientId, 0);
    mMovementHandler.setMoveRandom(0.f);

    const std::map<std::string, std::string> partsMap {
        {"whole", "light"}
    };
    const int typeId = 1;
    mMainCharData = &eCharsTextures::get(typeId);
    const auto& info = mMainCharData->charData();
    const float radius = info.radius();
    mMovementHandler.setRadius(radius);
    const auto modelParts = mMainCharData->mapToModelParts(partsMap);

    mRunAnimId = info.animId("run");
    mWalkAnimId = info.animId("walk");
    mWalkReadyAnimId = info.animId("walkReady");
    mStandAnimId = info.animId("stand");
    mStandReadyAnimId = info.animId("standReady");

    const auto model = mMainCharData->requestModel(modelParts, r);
    eCharUnitModel umodel;
    umodel.setCharModel(model);
    const int animId = chooseAnim(mStandAnimId, mStandReadyAnimId, false);
    umodel.setAnimation(animId, 1.f);
    umodel.setDirection(0);

    // const auto dir = "/home/ailuropoda/.eSlayer/tmp/preview/";
    // for(const auto& entry : std::filesystem::directory_iterator(dir))
    //     std::filesystem::remove_all(entry.path());
    // umodel.generatePreview(r);

    mMainChar = std::make_shared<eUnit>();
    mMainChar->setModel(umodel);
    mMainChar->fRadius = radius;
    mMainChar->fTypeId = typeId;
    mMainChar->fModelParts = modelParts;

    mStats.fSkills.emplace_back();
    mStats.fSkills.emplace_back();
}

void eMainCharAction::setPressedUnit(const std::shared_ptr<eUnit>& u) {
    if(u) mPressedItem = nullptr;
    mPressedUnit = u;
}

void eMainCharAction::setPressedItem(const std::shared_ptr<eGroundItem>& i) {
    if(i) mPressedUnit = nullptr;
    mPressedItem = i;
    mItemPickuped = false;
}

void eMainCharAction::increment(const bool mousePressed,
                                const bool rightPressed,
                                const bool shiftPressed,
                                const ePointF& mousePos,
                                const float by) {
    if(mMainChar->fHealth <= 0) return;

    auto& model = mMainChar->model();

    const eSkillChoice schoice{rightPressed ?
                       eSkillChoice::right :
                       eSkillChoice::left};

    handleAttackStop(mousePressed, rightPressed, shiftPressed);

    if(consumeActionTime(by, model)) return;

    const bool canUseSkill = mStats.canUseSkill(schoice);
    const bool rangeAttack = mStats.rangedAttack(schoice);

    ePointF targetPos = mousePos;
    bool shouldStopAttack = !canUseSkill;

    if(!shouldStopAttack) {
        if(mPressedUnit) {
            shouldStopAttack = !handleUnitAttack(schoice, model);
            targetPos = mPressedUnit->fPos;
        } else if(mousePressed && (shiftPressed || (rightPressed && rangeAttack))) {
            shouldStopAttack = !handlePositionAttack(mousePos, schoice, model);
        }
    }

    if(mPressedItem) {
        const auto itemId = mPressedItem->fItemId;
        const float dist = ePointF::distance(mPressedItem->fPos, mMainChar->fPos);
        if(!eInventoryWidget::sBlocked && dist < 0.5f) {
            if(!mItemPickuped) {
                const bool dragEnabled = eItemDragWidget::sInstance;
                mServer->pickupItem(mClientId, itemId, dragEnabled);
                eInventoryWidget::sBlocked = true;
                if(mousePressed) {
                    mItemPickuped = true;
                } else {
                    mPressedItem = nullptr;
                    mItemPickuped = false;
                }
                stop();
            }
            return;
        } else {
            targetPos = mPressedItem->fPos;
        }
    }

    if(shouldStopAttack) {
        stopAttack();
    }

    if(mAttackData.fType != eAttackTargetType::none) return;

    handleMovement(mousePressed, targetPos, by, model);
}

void eMainCharAction::handleAttackStop(
    const bool mousePressed,
    const bool rightPressed,
    const bool shiftPressed) {
    const auto atype = mAttackData.fType;

    const bool stop =
        (atype == eAttackTargetType::character && !mPressedUnit) ||
        (atype == eAttackTargetType::position && (!mousePressed || (!shiftPressed && !rightPressed)));

    if(stop) {
        stopAttack();
    }
}

bool eMainCharAction::consumeActionTime(
    const float by,
    eCharUnitModel& model) {
    const float prev = mMainChar->fActionTime;
    mMainChar->fActionTime -= by;

    if(prev > 0.f) {
        model.setAnimation(mMainChar->fAnim, mMainChar->fAnimSpeed);
        return true;
    }
    return false;
}

bool eMainCharAction::handleUnitAttack(
    const eSkillChoice schoice,
    eCharUnitModel& model) {
    const float attackDist = mStats.attackRange(
        schoice, mPressedUnit->fRadius,
        mMainChar->fRadius);

    const float dist = ePointF::distance(mMainChar->fPos, mPressedUnit->fPos);
    if(dist >= attackDist) {
        return false;
    }

    if(mAttackData.fType == eAttackTargetType::none) {
        const int targetId = mPressedUnit->fCharId;
        mAttackData = eAttackData(targetId, schoice);

        const auto vec = ePointF::vector(mPressedUnit->fPos, mMainChar->fPos);
        const float angle = vec.angle();
        mMainChar->fAngle = angle;
        model.setAngle(angle);

        mServer->attack(mClientId, mAttackData);
    }

    return true;
}

bool eMainCharAction::handlePositionAttack(
    const ePointF& mousePos,
    const eSkillChoice schoice,
    eCharUnitModel& model) {
    if(mAttackData.fType == eAttackTargetType::position &&
       ePointF::distance(mousePos, mAttackData.fPos) <= 0.1f) {
        return true;
    }

    mAttackData = eAttackData(mousePos, schoice);

    const auto vec = ePointF::vector(mousePos, mMainChar->fPos);
    const float angle = vec.angle();
    mMainChar->fAngle = angle;
    model.setAngle(angle);

    stand();
    mServer->attack(mClientId, mAttackData);
    return true;
}

void eMainCharAction::handleMovement(
    const bool mousePressed,
    const ePointF& pos,
    const float by,
    eCharUnitModel& model) {
    const bool run = shouldRun();
    mContinueRunning = false;
    updateWalkRunSpeed();

    bool moved = false;

    if(mousePressed) {
        mMovementHandler.moveInDirection(pos);
        moved = mMovementHandler.increment(by);
    }

    if(!moved) {
        if(mousePressed) mMovementHandler.moveTo(pos);
        moved = mMovementHandler.increment(by);
    }

    updateMovementAnimation(moved, run, by, model);
}

int eMainCharAction::chooseAnim(const int normal,
                                const int aggressive,
                                const bool isAggressive) {
    if(isAggressive) {
        return (aggressive != -1) ? aggressive : normal;
    } else {
        return (normal != -1) ? normal : aggressive;
    }
}

void eMainCharAction::updateMovementAnimation(
    const bool moved,
    const bool run,
    const float by,
    eCharUnitModel& model) {
    const bool aggressive = model.aggressive();
    int animId;

    if(moved) {
        mMainChar->fPos = mMovementHandler.pos();

        const float angle = mMovementHandler.angle();
        mMainChar->fAngle = angle;
        model.setAngle(angle);

        if(run) {
            mContinueRunning = true;
            incStamina(-by * 0.1f);
            animId = mRunAnimId;
        } else {
            incStamina(by * 0.05f);
            animId = chooseAnim(mWalkAnimId, mWalkReadyAnimId, aggressive);
        }
    } else {
        incStamina(by * 0.05f);
        animId = chooseAnim(mStandAnimId, mStandReadyAnimId, aggressive);
    }

    if(mMainChar->fAnim != animId) {
        mMainChar->fAnimId++;
    }

    mMainChar->fAnim = animId;
    mMainChar->fAnimSpeed = 1.f;
    model.setAnimation(animId, 1.f);
}

void eMainCharAction::stopAttack() {
    mPressedUnit = nullptr;
    mAttackData = eAttackData();
    mServer->stopAttack(mClientId);
}

void eMainCharAction::recalculateStats() {
    mStats.calculate(mAttributes, mEquipment);
    updateWalkRunSpeed();
}

void eMainCharAction::updateWalkRunSpeed() {
    const bool run = shouldRun();
    const float base = run ? 0.1f : 0.075f;
    mMovementHandler.setSpeed(base*(1.f + mStats.fWalkRun));
}

void eMainCharAction::mouseRelease(const ePointF& mousePos) {
    if(!mPressedUnit && !mPressedItem) {
        mMovementHandler.moveTo(mousePos);
    }
    if(mItemPickuped) {
        mItemPickuped = false;
        mPressedItem = nullptr;
    }
}

void eMainCharAction::stop() {
    mPressedUnit = nullptr;
    if(!mItemPickuped) mPressedItem = nullptr;
    if(mAttackData.fType != eAttackTargetType::none) {
        stopAttack();
    }
    mMovementHandler.stopMoving();
}

void eMainCharAction::incStamina(const float by) {
    mStamina = std::clamp(mStamina + by, 0.f, mMaxStamina);
}

bool eMainCharAction::rangedAttack(const eSkillChoice schoice) const {
    return mStats.rangedAttack(schoice);
}

void eMainCharAction::setSkillId(const eSkillChoice schoice, const int skillId) {
    auto& skillStats = mStats.skill(schoice);
    skillStats.fSkillId = skillId;
    recalculateStats();
}

void eMainCharAction::setAttributes(const eAttributes& attr) {
    mAttributes = attr;
    recalculateStats();
}

void eMainCharAction::setEquipment(const eEquipment& eq) {
    mEquipment = eq;
    recalculateStats();
}

void eMainCharAction::stand() {
    auto& model = mMainChar->model();

    const bool aggressive = model.aggressive();
    const int animId = chooseAnim(mStandAnimId, mStandReadyAnimId, aggressive);

    mMainChar->fAnim = animId;
    mMainChar->fAnimId++;
    mMainChar->fAnimSpeed = 1.f;
    model.setAnimation(animId, 1.f);
}

bool eMainCharAction::shouldRun() const {
    if(!mRunning) return false;
    return mStamina > 5.f || (mStamina > 0.f && mContinueRunning);
}

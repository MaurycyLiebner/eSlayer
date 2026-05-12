#include "emaincharaction.h"

#include "textures/echarstextures.h"
#include "textures/echartextures.h"
#include "units/eunit.h"

#include "widgets/gameScreen/einventorywidget.h"
#include "widgets/gameScreen/ehoverwidget.h"

#include <eSlayerServer/eserver.h>

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/eobjectsinfo.h>

eMainCharAction::eMainCharAction() :
    mMainChar(std::make_shared<eUnit>()),
    mMovementHandler(mMainChar->fPos,
                     mMainChar->fAngle) {}

void eMainCharAction::initialize(const std::shared_ptr<eServer>& s,
                                 const eResolution& res,
                                 SDL_Renderer* const r,
                                 const eWalkable& w,
                                 const eOtherIterator& iter,
                                 const int clientId,
                                 const eTeamId teamId) {
    mClientId = clientId;
    mServer = s;
    mMovementHandler.intialize(w, iter, clientId, teamId);
    mMovementHandler.setMoveRandom(0.f);

    const std::map<std::string, std::string> partsMap {
        {"whole", "light"}
    };
    const int typeId = 0;
    const auto& udata = eUnitsInfo::sUnits.get(typeId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    mMainCharTexs = &eCharsTextures::get(typeId);
    const float radius = udata.fRadius;
    mMovementHandler.setRadius(radius);
    const auto modelParts = mMainCharTexs->mapToModelParts(partsMap);

    mRunAnimId = data.animId("run");
    mWalkAnimId = data.animId("walk");
    mWalkReadyAnimId = data.animId("walkReady");
    mStandAnimId = data.animId("stand");
    mStandReadyAnimId = data.animId("standReady");

    const auto model = mMainCharTexs->requestModel(
        modelParts, res, r);
    eCharUnitModel umodel;
    umodel.setCharModel(model);
    const int animId = eMovementHandler::sChooseAnim(
        mStandAnimId, mStandReadyAnimId, false);
    umodel.setAnimation(animId, 1.f);
    umodel.setDirection(0);

    // const auto dir = "/home/ailuropoda/.eSlayer/tmp/preview/";
    // for(const auto& entry : std::filesystem::directory_iterator(dir))
    //     std::filesystem::remove_all(entry.path());
    // umodel.generatePreview(r);

    mMainChar->setModel(umodel);
    mMainChar->fRadius = radius;
    mMainChar->fCharDataId = typeId;
    mMainChar->fModelParts = modelParts;
    mMainChar->fTeamId = teamId;

    mStats.fSkills.emplace_back();
    mStats.fSkills.emplace_back();
}

void eMainCharAction::setPressedUnit(
    const std::shared_ptr<eUnit>& u) {
    if(u) {
        mPressedItem.reset();
        mPressedObject.reset();
        mPressedDoors = std::nullopt;
    }
    mPressedUnit = u;
}

void eMainCharAction::setPressedItem(
    const std::shared_ptr<eGroundItem>& i) {
    if(i) {
        mPressedUnit.reset();
        mPressedObject.reset();
        mPressedDoors = std::nullopt;
    }
    mPressedItem = i;
}

void eMainCharAction::setPressedObject(
    const std::shared_ptr<eObject>& o) {
    if(o) {
        mPressedItem.reset();
        mPressedUnit.reset();
        mPressedDoors = std::nullopt;
    }
    mPressedObject = o;
}

void eMainCharAction::setPressedDoors(
    const std::optional<eDoors>& d) {
    if(d) {
        mPressedItem.reset();
        mPressedUnit.reset();
        mPressedObject.reset();
    }
    mPressedDoors = d;
}

void eMainCharAction::increment(const bool mousePressed,
                                const bool rightPressed,
                                const bool shiftPressed,
                                const ePointF& mousePos,
                                const float by) {
    if(mClickAction) return;

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

    const auto& charPos = mMainChar->fPos;

    if(!shouldStopAttack) {
        if(const auto u = mPressedUnit.lock()) {
            const auto& upos = u->fPos;
            targetPos = upos;
            if(u->isBody()) {
                const float dist = ePointF::distance(upos, charPos);
                if(dist < 0.5f) {
                    const auto bodyId = u->fCharId;
                    mServer->pickupBody(mClientId, bodyId);
                    eInventoryWidget::sBlocked = true;
                    stop();
                }
            } else {
                const bool attacked = handleUnitAttack(*u, schoice, model);
                if(attacked && !mousePressed) {
                    stop();
                } else if(!attacked) {
                    // Target is out of range (e.g. knocked back),
                    // clear any active attack so we walk toward it
                    if(mAttackData.fType != eAttackTargetType::none) {
                        mAttackData = eAttackData();
                        mServer->stopAttack(mClientId);
                    }
                }
            }
        } else if(mousePressed && (shiftPressed || (rightPressed && rangeAttack))) {
            shouldStopAttack = !handlePositionAttack(mousePos, schoice, model);
        }
    }

    if(const auto item = mPressedItem.lock()) {
        const auto itemId = item->fItemId;
        const auto& ipos = item->fPos;
        const float dist = ePointF::distance(ipos, charPos);
        if(!eInventoryWidget::sBlocked && dist < 0.5f) {
            const bool dragEnabled = eInventoryWidget::sInstance;
            mServer->pickupItem(mClientId, itemId, dragEnabled);
            eInventoryWidget::sBlocked = true;
            mClickAction = mousePressed;
            stop();
            return;
        } else {
            targetPos = ipos;
        }
    } else if(const auto object = mPressedObject.lock()) {
        const auto objectId = object->fObjectId;
        const auto type = object->fObjectType;
        const auto& info = eObjectsInfo::sObjects.get(type);
        const auto& opos = object->fPos;
        const float size = info.fSize;
        const ePointF t{opos};
        const ePointF tr{opos.fX + 0.5f*size, opos.fY};
        const ePointF r{opos.fX + size, opos.fY};
        const ePointF br{opos.fX + size, opos.fY + 0.5f*size};
        const ePointF b{opos.fX + size, opos.fY + size};
        const ePointF bl{opos.fX + 0.5f*size, opos.fY + size};
        const ePointF l{opos.fX, opos.fY + size};
        const ePointF tl{opos.fX, opos.fY + 0.5f*size};
        ePointF closesPos = t;
        float minDist = ePointF::distance(t, charPos);
        const auto handlePos = [&](const ePointF& pos) {
            const float dist = ePointF::distance(pos, charPos);
            if(dist < minDist) {
                closesPos = pos;
                minDist = dist;
            }
        };
        handlePos(tr);
        handlePos(r);
        handlePos(br);
        handlePos(b);
        handlePos(bl);
        handlePos(l);
        handlePos(tl);
        if(minDist < 0.5f) {
            const int tx = opos.fX;
            const int ty = opos.fY;
            mServer->triggerObject(mClientId, objectId, tx, ty);
            mClickAction = mousePressed;
            stop();
            return;
        } else {
            targetPos = closesPos;
        }
    } else if(mPressedDoors) {
        const auto& d = *mPressedDoors;
        const auto doorsPos = d.pos();

        const float dist = ePointF::distance(doorsPos, charPos);
        if(dist < 0.5f) {
            mServer->triggerDoors(mClientId, d);
            mClickAction = mousePressed;
            stop();
            return;
        } else {
            targetPos = doorsPos;
        }
    }

    if(shouldStopAttack && mAttackData.fType != eAttackTargetType::none) {
        stopAttack();
    }

    if(mAttackData.fType != eAttackTargetType::none) return;

    const bool hasPressedUnit = !mPressedUnit.expired();
    handleMovement(mousePressed || hasPressedUnit, targetPos, by, model);
}

void eMainCharAction::mousePress() {
    stop();
}

void eMainCharAction::handleAttackStop(
    const bool mousePressed,
    const bool rightPressed,
    const bool shiftPressed) {
    const auto atype = mAttackData.fType;

    const bool stop =
        (atype == eAttackTargetType::character && mPressedUnit.expired()) ||
        (atype == eAttackTargetType::position && (!mousePressed || (!shiftPressed && !rightPressed)));

    if(stop) {
        stopAttack();
    }
}

bool eMainCharAction::consumeActionTime(
    const float by,
    eCharUnitModel& model) {
    const float prev = mMainChar->fBlockingActionTime;
    mMainChar->fBlockingActionTime -= by;

    if(prev > 0.f) {
        const auto anim = mMainChar->fAnim;
        const float speed = mMainChar->fAnimSpeed;
        model.setAnimation(anim, speed);
        return true;
    }
    return false;
}

bool eMainCharAction::handleUnitAttack(
    eUnit& u,
    const eSkillChoice schoice,
    eCharUnitModel& model) {
    const bool rangedAttack = mStats.rangedAttack(schoice);
    if(!rangedAttack) {
        const float attackDist = mStats.attackRange(
            schoice, u.fRadius, mMainChar->fRadius);

        const float dist = ePointF::distance(mMainChar->fPos, u.fPos);
        if(dist >= attackDist) {
            return false;
        }
    }

    if(mAttackData.fType == eAttackTargetType::none) {
        const int targetId = u.fCharId;
        mAttackData = eAttackData(targetId, schoice);

        const auto vec = ePointF::vector(u.fPos, mMainChar->fPos);
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

void eMainCharAction::updateMovementAnimation(
    const bool moved,
    const bool run,
    const float by,
    eCharUnitModel& model) {
    const bool aggressive = model.aggressive();
    int animId;

    float speed = 1.f;

    if(moved) {
        mMainChar->fPos = mMovementHandler.pos();

        const float angle = mMovementHandler.angle();
        speed = 0.5f*(1.f + mStats.fWalkRun);
        mMainChar->fAngle = angle;
        model.setAngle(angle);

        if(run) {
            mContinueRunning = true;
            incStamina(-by * 0.1f);
            animId = mRunAnimId;
        } else {
            incStamina(by * 0.1f);
            animId = eMovementHandler::sChooseAnim(mWalkAnimId, mWalkReadyAnimId, aggressive);
        }
    } else {
        incStamina(by * 0.1f);
        animId = eMovementHandler::sChooseAnim(mStandAnimId, mStandReadyAnimId, aggressive);
    }

    if(mMainChar->fAnim != animId) {
        mMainChar->fAnimId++;
    }

    mMainChar->fAnim = animId;
    mMainChar->fAnimSpeed = speed;
    model.setAnimation(animId, speed);
}

void eMainCharAction::stopAttack() {
    mPressedUnit.reset();
    mAttackData = eAttackData();
    mServer->stopAttack(mClientId);
}

void eMainCharAction::recalculateSkillStats(const int schoice) {
    mStats.calculateSkill(schoice, mEquipment);
}

void eMainCharAction::recalculateStats() {
    mStats.calculate(mAttributes, mEquipment);
    updateWalkRunSpeed();
}

void eMainCharAction::updateWalkRunSpeed() {
    const bool run = shouldRun();
    const float base = run ? 0.1f : 0.07f;
    const float speed = base*(1.f + mStats.fWalkRun);
    mMovementHandler.setSpeed(speed);
}

void eMainCharAction::mouseRelease(const ePointF& mousePos) {
    if(mClickAction) {
        mClickAction = false;
    } else if(mPressedUnit.expired() &&
              mPressedItem.expired() &&
              mPressedObject.expired()) {
        mMovementHandler.moveTo(mousePos);
    }
}

void eMainCharAction::stop() {
    mPressedUnit.reset();
    mPressedItem.reset();
    mPressedObject.reset();
    mPressedDoors.reset();
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
    recalculateSkillStats(static_cast<int>(schoice));
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
    const int animId = eMovementHandler::sChooseAnim(mStandAnimId, mStandReadyAnimId, aggressive);

    mMainChar->fAnim = animId;
    mMainChar->fAnimId++;
    mMainChar->fAnimSpeed = 1.f;
    model.setAnimation(animId, 1.f);
}

bool eMainCharAction::shouldRun() const {
    if(!mRunning) return false;
    return mStamina > 5.f || (mStamina > 0.f && mContinueRunning);
}

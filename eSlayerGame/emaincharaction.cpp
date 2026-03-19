#include "emaincharaction.h"

#include "textures/echarstextures.h"
#include "textures/echartextures.h"
#include "units/eunit.h"

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
    mMovementHandler.setRadius(0.4f);
    mMovementHandler.setMoveRandom(0.f);

    const std::map<std::string, std::string> partsMap {
        {"whole", "light"}
    };
    const int typeId = 1;
    mMainCharData = eCharsTextures::get(typeId);
    const auto modelParts = mMainCharData->mapToModelParts(partsMap);

    const auto model = mMainCharData->generateModel(modelParts, r);
    eCharUnitModel umodel;
    umodel.setCharModel(model);
    umodel.setAnimation(0, 1.f);
    umodel.setDirection(0);

    mMainChar = std::make_shared<eUnit>();
    mMainChar->setModel(umodel);
    mMainChar->fRadius = 0.4f;
    mMainChar->fTypeId = typeId;
    mMainChar->fModelParts = modelParts;
}

void eMainCharAction::setPressedUnit(const std::shared_ptr<eUnit>& u) {
    mPressedUnit = u;
}

void eMainCharAction::increment(const bool mousePressed,
                                const bool shiftPressed,
                                const ePointF& mousePos,
                                const uint8_t skill,
                                const float by) {
    if(mMainChar->fHealth <= 0) return;

    auto& model = mMainChar->model();

    const auto atype = mAttackData.fType;
    bool stopAttack = (atype == eAttackTargetType::character && !mPressedUnit.get()) ||
                      (atype == eAttackTargetType::position && (!mousePressed || !shiftPressed));

    if(stopAttack) {
        mAttackData = eAttackData();
        mServer->stopAttack(mClientId);
        stopAttack = false;
    }

    const float tmp = mMainChar->fActionTime;
    mMainChar->fActionTime -= by;
    if(tmp > 0.f) {
        model.setAnimation(mMainChar->fAnim,
                           mMainChar->fAnimSpeed);
        return;
    }

    ePointF pos;

    if(mPressedUnit) {
        pos = mPressedUnit->fPos;
        const float dist = ePointF::distance(mMainChar->fPos, pos);
        const float attackDist = 0.5f*(mPressedUnit->fRadius + mMainChar->fRadius);

        if(dist < attackDist) {
            if(mAttackData.fType != eAttackTargetType::none) {
            } else {
                const int targetId = mPressedUnit->fCharId;
                mAttackData = eAttackData(targetId, skill);
                const auto vec = ePointF::vector(mPressedUnit->fPos,
                                                 mMainChar->fPos);
                model.setAngle(vec.angle());
                mServer->attack(mClientId, mAttackData);
            }
        } else {
            stopAttack = true;
        }
    } else if(mousePressed && shiftPressed) {
        if(mAttackData.fType != eAttackTargetType::position ||
           ePointF::distance(mousePos, mAttackData.fPos) > 0.1f) {
            mAttackData = eAttackData(mousePos, skill);
            const auto vec = ePointF::vector(mousePos,
                                             mMainChar->fPos);
            model.setAngle(vec.angle());
            mServer->attack(mClientId, mAttackData);
        }
    } else {
        pos = mousePos;
    }

    if(stopAttack) {
        mAttackData = eAttackData();
        mServer->stopAttack(mClientId);
    }

    if(mAttackData.fType != eAttackTargetType::none) return;

    const bool run = shouldRun();
    mContinueRunning = false;
    if(run) {
        mMovementHandler.setSpeed(0.1f);
    } else {
        mMovementHandler.setSpeed(0.075f);
    }

    bool move = false;
    if(mousePressed) {
        mMovementHandler.moveInDirection(pos);
        move = mMovementHandler.increment(1.f);
    }
    if(!move) {
        if(mousePressed) mMovementHandler.moveTo(pos);
        move = mMovementHandler.increment(1.f);
    }

    const bool a = model.aggressive();
    int animId;
    if(move) {
        mMainChar->fPos = mMovementHandler.pos();
        const float angle = mMovementHandler.angle();
        mMainChar->fAngle = angle;
        model.setAngle(angle);
        if(run) {
            mContinueRunning = true;
            incStamina(-0.1f);
            animId = mMainCharData->animId("run");
        } else {
            incStamina(0.05f);
            const int naId = mMainCharData->animId("walk");
            const int aId = mMainCharData->animId("walkReady");
            if(a) {
                if(aId != -1) {
                    animId = aId;
                } else {
                    animId = naId;
                }
            } else {
                if(naId != -1) {
                    animId = naId;
                } else {
                    animId = aId;
                }
            }
        }
    } else {
        incStamina(0.05f);
        const int naId = mMainCharData->animId("stand");
        const int aId = mMainCharData->animId("standReady");
        if(a) {
            if(aId != -1) {
                animId = aId;
            } else {
                animId = naId;
            }
        } else {
            if(naId != -1) {
                animId = naId;
            } else {
                animId = aId;
            }
        }
    }
    if(mMainChar->fAnim != animId) {
        mMainChar->fAnimId++;
    }
    mMainChar->fAnim = animId;
    mMainChar->fAnimSpeed = 1.f;
    model.setAnimation(animId, 1.f);
}

void eMainCharAction::mouseRelease(const ePointF& mousePos) {
    if(mPressedUnit) {
        mPressedUnit = nullptr;
    } else {
        mMovementHandler.moveTo(mousePos);
    }
}

void eMainCharAction::stop() {
    mPressedUnit = nullptr;
    if(mAttackData.fType != eAttackTargetType::none) {
        mServer->stopAttack(mClientId);
        mAttackData = eAttackData();
    }
    mMovementHandler.stopMoving();
}

void eMainCharAction::incStamina(const float by) {
    mStamina = std::clamp(mStamina + by, 0.f, mMaxStamina);
}

bool eMainCharAction::shouldRun() const {
    if(!mRunning) return false;
    return mStamina > 5.f || (mStamina > 0.f && mContinueRunning);
}

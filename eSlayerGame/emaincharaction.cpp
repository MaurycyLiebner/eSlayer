#include "emaincharaction.h"

#include "units/eunit.h"
#include "textures/echartextures.h"
#include "textures/echarstextures.h"

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
    mMovementHandler.setRadius(0.4);
    mMovementHandler.setMoveRandom(0.);

    const eCharTextures::eModelParts modelParts {
        {"whole", "light"}
    };
    mMainCharData = eCharsTextures::get("pal");

    const auto model = mMainCharData->generateModel(modelParts, r);
    eCharUnitModel umodel;
    umodel.setCharModel(model);
    umodel.setAnimation(0, 1.);
    umodel.setDirection(0);

    mMainChar = std::make_shared<eUnit>();
    mMainChar->setModel(umodel);
    mMainChar->fRadius = 0.4;
}

void eMainCharAction::setPressedUnit(const std::shared_ptr<eUnit>& u) {
    mPressedUnit = u;
}

void eMainCharAction::increment(const bool mousePressed,
                                const ePointF& mousePos,
                                const double by) {
    if(mMainChar->fHealth <= 0) return;

    auto& model = mMainChar->model();

    bool stopAttack = mAttack && !mPressedUnit.get();

    if(stopAttack) {
        mAttack = false;
        mServer->stopAttack(mClientId);
        stopAttack = false;
    }

    const double tmp = mMainChar->fActionTime;
    mMainChar->fActionTime -= by;
    if(tmp > 0.) {
        model.setAnimation(mMainChar->fAnim, mMainChar->fAnimId,
                           mMainChar->fAnimSpeed);
        return;
    }

    ePointF pos;

    if(mPressedUnit) {
        pos = mPressedUnit->fPos;
        const double dist = ePointF::distance(mMainChar->fPos, pos);
        const double attackDist = 0.5*(mPressedUnit->fRadius + mMainChar->fRadius);

        if(dist < attackDist) {
            if(mAttack) {
            } else {
                mAttack = true;
                const auto vec = ePointF::vector(mPressedUnit->fPos,
                                                 mMainChar->fPos);
                model.setAngle(vec.angle());
                const int targetId = mPressedUnit->fCharId;
                mServer->attack(mClientId, targetId);
            }
        } else {
            stopAttack = true;
        }
    } else {
        pos = mousePos;
    }

    if(stopAttack) {
        mAttack = false;
        mServer->stopAttack(mClientId);
    }

    if(mAttack) return;

    const bool run = shouldRun();
    mContinueRunning = false;
    if(run) {
        mMovementHandler.setSpeed(0.1);
    } else {
        mMovementHandler.setSpeed(0.075);
    }

    bool move = false;
    if(mousePressed) {
        mMovementHandler.moveInDirection(pos);
        move = mMovementHandler.increment(1.);
    }
    if(!move) {
        if(mousePressed) mMovementHandler.moveTo(pos);
        move = mMovementHandler.increment(1.);
    }

    const bool a = model.aggressive();
    int animId;
    if(move) {
        mMainChar->fPos = mMovementHandler.pos();
        const double angle = mMovementHandler.angle();
        mMainChar->fAngle = angle;
        model.setAngle(angle);
        if(run) {
            mContinueRunning = true;
            incStamina(-0.1);
            animId = mMainCharData->animId("run");
        } else {
            incStamina(0.05);
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
        incStamina(0.05);
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
    model.setAnimation(animId, 1.);
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
    mAttack = false;
    mMovementHandler.stopMoving();
}

void eMainCharAction::incStamina(const double by) {
    mStamina = std::clamp(mStamina + by, 0., mMaxStamina);
}

bool eMainCharAction::shouldRun() const {
    if(!mRunning) return false;
    return mStamina > 5. || (mStamina > 0. && mContinueRunning);
}

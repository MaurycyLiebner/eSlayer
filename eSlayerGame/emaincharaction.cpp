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
    auto& model = mMainChar->model();

    bool stopAttack = mAttack && !mPressedUnit.get();

    if(stopAttack) {
        mAttack = false;
        mServer->stopAttack(mClientId);
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
        pos = mPressedUnit->pos();
        const double dist = ePointF::distance(mMainChar->pos(), pos);
        const double attackDist = 0.5*(mPressedUnit->fRadius + mMainChar->fRadius);

        if(dist < attackDist) {
            if(mAttack) {
            } else {
                mAttack = true;
                const auto vec = ePointF::vector(mPressedUnit->pos(),
                                                 mMainChar->pos());
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
        model.setAngle(mMovementHandler.angle());
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
    } else {
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

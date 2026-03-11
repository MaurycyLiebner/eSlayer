#include "emaincharaction.h"

#include "units/eunit.h"
#include "textures/echartextures.h"
#include "textures/echarstextures.h"

void eMainCharAction::initialize(SDL_Renderer* const r,
                                 const eWalkable& w,
                                 const eOtherIterator& iter,
                                 const int clientId,
                                 const int teamId) {
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
    umodel.setAnimation(0);
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
    ePointF pos;
    bool move = false;
    eVec2d vec;
    auto& model = mMainChar->model();

    mAttackTime -= by;
    if(mPressedUnit) {
        pos = mPressedUnit->pos();
        const double dist = ePointF::distance(mMainChar->pos(), pos);
        const double attackDist = 0.5*(mPressedUnit->fRadius + mMainChar->fRadius);

        if(dist < attackDist) {
            if(mAttackTime <= 0.) {
                int animId;
                const int a1Id = mMainCharData->animId("attack1");
                const int a2Id = mMainCharData->animId("attack2");
                if(eRand::rand() % 2) {
                    if(a1Id != -1) {
                        animId = a1Id;
                    } else {
                        animId = a2Id;
                    }
                } else {
                    if(a2Id != -1) {
                        animId = a2Id;
                    } else {
                        animId = a1Id;
                    }
                }
                model.setAnimation(animId);
                mAttackTime = mMainCharData->animFrames(animId);
                const auto vec = ePointF::vector(mPressedUnit->pos(),
                                                 mMainChar->pos());
                model.setAngle(vec.angle());
                mAttackedUnit = mPressedUnit;
            }
        }
    } else {
        pos = mousePos;
    }
    if(mAttackTime <= 0.) {
        mAttackedUnit = nullptr;

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
        model.setAnimation(animId);
    }
}

void eMainCharAction::mouseRelease(const ePointF& mousePos) {
    if(mPressedUnit) {
        mPressedUnit = nullptr;
    } else {
        mMovementHandler.moveTo(mousePos);
    }
}

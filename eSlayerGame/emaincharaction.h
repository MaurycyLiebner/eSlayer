#ifndef EMAINCHARACTION_H
#define EMAINCHARACTION_H

#include <eSlayerHelpers/emovementhandler.h>

#include <memory>

class eUnit;
class SDL_Renderer;
class eCharTextures;

class eMainCharAction {
public:
    void initialize(SDL_Renderer* const r,
                    const eWalkable& w,
                    const eOtherIterator& iter,
                    const int clientId,
                    const int teamId);

    void setPressedUnit(const std::shared_ptr<eUnit>& u);

    void increment(const bool mousePressed,
                   const ePointF& mousePos,
                   const double by);

    void mouseRelease(const ePointF& mousePos);

    const ePointF& pos() const { return mMovementHandler.pos(); }
    const std::shared_ptr<eUnit>& unit() const { return mMainChar; }
private:
    std::shared_ptr<eUnit> mPressedUnit;
    std::shared_ptr<eUnit> mMainChar;
    eCharTextures* mMainCharData = nullptr;
    eMovementHandler mMovementHandler;

    double mAttackTime = 0.;
    std::shared_ptr<eUnit> mAttackedUnit;
};

#endif // EMAINCHARACTION_H

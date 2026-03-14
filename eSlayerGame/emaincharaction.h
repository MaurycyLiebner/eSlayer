#ifndef EMAINCHARACTION_H
#define EMAINCHARACTION_H

#include <eSlayerHelpers/emovementhandler.h>

#include <memory>

class eUnit;
class SDL_Renderer;
class eCharTextures;
class eServer;

class eMainCharAction {
public:
    void initialize(const std::shared_ptr<eServer>& s,
                    SDL_Renderer* const r,
                    const eWalkable& w,
                    const eOtherIterator& iter,
                    const int clientId,
                    const int teamId);

    void setPressedUnit(const std::shared_ptr<eUnit>& u);

    void increment(const bool mousePressed,
                   const ePointF& mousePos,
                   const double by);

    void mouseRelease(const ePointF& mousePos);
    void stop();

    const ePointF& pos() const { return mMovementHandler.pos(); }
    void setPos(const ePointF& pos) { mMovementHandler.setPos(pos); }
    const std::shared_ptr<eUnit>& unit() const { return mMainChar; }
private:
    int mClientId;
    std::shared_ptr<eServer> mServer;
    std::shared_ptr<eUnit> mPressedUnit;
    std::shared_ptr<eUnit> mMainChar;
    eCharTextures* mMainCharData = nullptr;
    eMovementHandler mMovementHandler;

    bool mAttack = false;
};

#endif // EMAINCHARACTION_H

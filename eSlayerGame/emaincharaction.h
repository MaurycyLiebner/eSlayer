#ifndef EMAINCHARACTION_H
#define EMAINCHARACTION_H

#include <eSlayerHelpers/eattackdata.h>
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
                   const bool rightPressed,
                   const bool shiftPressed,
                   const ePointF& mousePos,
                   const int skillId,
                   const float by);

    void mouseRelease(const ePointF& mousePos);
    void stop();

    const ePointF& pos() const { return mMovementHandler.pos(); }
    void setPos(const ePointF& pos) { mMovementHandler.setPos(pos); }
    const std::shared_ptr<eUnit>& unit() const { return mMainChar; }

    float stamina() const { return mStamina; }
    void incStamina(const float by);
    float maxStamina() const { return mMaxStamina; }
    bool running() const { return mRunning; }
    void setRunning(const bool r) { mRunning = r; }
private:
    bool shouldRun() const;

    int mClientId;
    std::shared_ptr<eServer> mServer;
    std::shared_ptr<eUnit> mPressedUnit;
    std::shared_ptr<eUnit> mMainChar;
    eCharTextures* mMainCharData = nullptr;
    eMovementHandler mMovementHandler;
    eAttackData mAttackData;

    bool mRunning = false;
    float mMaxStamina = 100.f;
    float mStamina = mMaxStamina;
    bool mContinueRunning = false;
};

#endif // EMAINCHARACTION_H

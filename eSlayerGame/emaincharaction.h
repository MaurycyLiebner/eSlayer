#ifndef EMAINCHARACTION_H
#define EMAINCHARACTION_H

#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/estats.h>

#include <memory>

class eUnit;
class SDL_Renderer;
class eCharTextures;
class eServer;
class eCharUnitModel;
class eSkill;

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
                   const float by);

    void mouseRelease(const ePointF& mousePos);
    void stop();
    void stand();

    const ePointF& pos() const { return mMovementHandler.pos(); }
    void setPos(const ePointF& pos) { mMovementHandler.setPos(pos); }
    const std::shared_ptr<eUnit>& unit() const { return mMainChar; }

    float stamina() const { return mStamina; }
    void incStamina(const float by);
    float maxStamina() const { return mMaxStamina; }
    bool running() const { return mRunning; }
    void setRunning(const bool r) { mRunning = r; }

    bool rangedAttack(const eSkillChoice schoice) const;

    void setSkillId(const eSkillChoice schoice, const int skillId);
    void setAttributes(const eAttributes& attr);
    void setEquipment(const eEquipment& eq);
    eEquipment& equipment() { return mEquipment; }
    eAttributes& attributes() { return mAttributes; }
    eStats& stats() { return mStats; }
    void recalculateStats();
private:
    void updateWalkRunSpeed();
    bool shouldRun() const;
    void handleAttackStop(const bool mousePressed,
                          const bool rightPressed,
                          const bool shiftPressed);

    bool consumeActionTime(const float by,
                           eCharUnitModel& model);

    bool handleUnitAttack(const eSkillChoice schoice,
                          eCharUnitModel& model);

    bool handlePositionAttack(const ePointF& mousePos,
                              const eSkillChoice schoice,
                              eCharUnitModel& model);

    void handleMovement(const bool mousePressed,
                        const ePointF& pos,
                        const float by,
                        eCharUnitModel& model);

    int chooseAnim(const int normal,
                   const int aggressive,
                   const bool isAggressive);

    void updateMovementAnimation(const bool moved,
                                 const bool run,
                                 const float by,
                                 eCharUnitModel& model);

    void stopAttack();

    int mClientId;
    std::shared_ptr<eServer> mServer;
    std::shared_ptr<eUnit> mPressedUnit;
    std::shared_ptr<eUnit> mMainChar;
    eCharTextures* mMainCharData = nullptr;
    eMovementHandler mMovementHandler;
    eAttackData mAttackData;

    eEquipment mEquipment;
    eAttributes mAttributes;
    eStats mStats;

    bool mRunning = false;
    float mMaxStamina = 100.f;
    float mStamina = mMaxStamina;
    bool mContinueRunning = false;

    int mRunAnimId = -1;
    int mWalkAnimId = -1;
    int mWalkReadyAnimId = -1;
    int mStandAnimId = -1;
    int mStandReadyAnimId = -1;
};

#endif // EMAINCHARACTION_H

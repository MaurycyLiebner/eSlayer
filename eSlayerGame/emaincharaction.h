#ifndef EMAINCHARACTION_H
#define EMAINCHARACTION_H

#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/ewalldirection.h>
#include <eSlayerHelpers/edoors.h>

#include <memory>

class eUnit;
class SDL_Renderer;
class eCharTextures;
class eServer;
class eCharUnitModel;
class eSkill;
class eResolution;

class eMainCharAction {
public:
    eMainCharAction();

    void initialize(const std::shared_ptr<eServer>& s,
                    const eResolution& res,
                    SDL_Renderer* const r,
                    const eWalkable& w,
                    const eOtherIterator& iter,
                    const int clientId,
                    const eTeamId teamId);

    void setPressedUnit(const std::shared_ptr<eUnit>& u);
    void setPressedItem(const std::shared_ptr<eGroundItem>& i);
    void setPressedObject(const std::shared_ptr<eObject>& o);
    void setPressedDoors(const std::optional<eDoors>& d);

    void increment(const bool mousePressed,
                   const bool rightPressed,
                   const bool shiftPressed,
                   const ePointF& mousePos,
                   const float by);

    void mousePress();
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
    void recalculateSkillStats(const int schoice);
    void recalculateStats();
private:
    void updateWalkRunSpeed();
    bool shouldRun() const;
    void handleAttackStop(const bool mousePressed,
                          const bool rightPressed,
                          const bool shiftPressed);

    bool consumeActionTime(const float by,
                           eCharUnitModel& model);

    bool handleUnitAttack(eUnit& u,
                          const eSkillChoice schoice,
                          eCharUnitModel& model);

    bool handlePositionAttack(const ePointF& mousePos,
                              const eSkillChoice schoice,
                              eCharUnitModel& model);

    void handleMovement(const bool mousePressed,
                        const ePointF& pos,
                        const float by,
                        eCharUnitModel& model);

    void updateMovementAnimation(const bool moved,
                                 const bool run,
                                 const float by,
                                 eCharUnitModel& model);

    void stopAttack();

    int mClientId;
    std::shared_ptr<eServer> mServer;

    std::weak_ptr<eUnit> mPressedUnit;
    std::weak_ptr<eGroundItem> mPressedItem;
    std::weak_ptr<eObject> mPressedObject;
    std::optional<eDoors> mPressedDoors;

    std::shared_ptr<eUnit> mMainChar;
    eCharTextures* mMainCharTexs = nullptr;
    eMovementHandler mMovementHandler;
    eAttackData mAttackData;

    eEquipment mEquipment;
    eAttributes mAttributes;
    eStats mStats;

    bool mClickAction = false;

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

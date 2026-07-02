#ifndef EMAINCHARACTION_H
#define EMAINCHARACTION_H

#include "emovementhandler.h"

#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/edoors.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eslayerquests.h>
#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/ewalldirection.h>
#include <eSlayerHelpers/etalkheard.h>

#include <SDL3/SDL_rect.h>

#include <memory>

class eMap;
class eUnit;
class SDL_Renderer;
class eCharTextures;
class eServer;
class eCharUnitModel;
class eSkill;
class eResolution;

class eMainCharAction {
public:
    eMainCharAction(ePathFinderMap& map);

    void initialize(const std::shared_ptr<eServer>& s,
                    const eResolution& res,
                    SDL_Renderer* const r,
                    const std::shared_ptr<eMap>& map,
                    const eOtherIterator& iter,
                    const uint32_t clientId,
                    const eTeamId teamId);

    void setPressedUnit(const std::shared_ptr<eUnit>& u);
    void setPressedItem(const std::shared_ptr<eGroundItem>& i);
    void setPressedObject(const std::shared_ptr<eObject>& o);
    void setPressedDoors(const std::optional<eDoors>& d);
    void setPressedStairs(const std::optional<eStairs>& s);

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
    const std::shared_ptr<eUnit>& unit() const { return mMainChar; }

    float stamina() const { return mStats.fStaminaF; }
    bool running() const { return mRunning; }
    void setRunning(const bool r) { mRunning = r; }

    bool rangedAttack(const eSkillChoice schoice) const;

    void setSkillId(const eSkillChoice schoice, const int skillId);
    void setAttributes(const eAttributes& attr);
    void setEquipment(const eEquipment& eq);
    eEquipment& equipment() { return mEquipment; }
    eAttributes& attributes() { return mAttributes; }
    eStats& stats() { return mStats; }
    eSlayerQuests& quests() { return mQuests; }
    eTalkHeard& talkHeard() { return mTalkHeard; }
    void recalculateStats();
private:
    void openMainMenu(
        const uint32_t sellerId,
        const std::string& baseName,
        const std::string& name,
        const SDL_Rect& rect);
    bool tryOpenTalk(const uint32_t sellerId,
                     const std::string& baseName,
                     const std::string& name,
                     const SDL_Rect& rect);

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

    uint32_t mClientId;
    std::shared_ptr<eServer> mServer;
    std::shared_ptr<eMap> mMap;

    std::weak_ptr<eUnit> mPressedUnit;
    std::weak_ptr<eGroundItem> mPressedItem;
    std::weak_ptr<eObject> mPressedObject;
    std::optional<eDoors> mPressedDoors;
    std::optional<eStairs> mPressedStairs;

    std::shared_ptr<eUnit> mMainChar;
    eCharTextures* mMainCharTexs = nullptr;
    eMovementHandler mMovementHandler;
    eAttackData mAttackData;

    eEquipment mEquipment;
    eAttributes mAttributes;
    eSlayerQuests mQuests;
    eTalkHeard mTalkHeard;
    eStats mStats;

    bool mClickAction = false;

    bool mRunning = false;
    bool mContinueRunning = false;

    int mRunAnimId = -1;
    int mWalkAnimId = -1;
    int mWalkReadyAnimId = -1;
    int mStandAnimId = -1;
    int mStandReadyAnimId = -1;
};

#endif // EMAINCHARACTION_H

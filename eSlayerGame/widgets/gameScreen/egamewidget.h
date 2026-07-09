#ifndef EGAMEWIDGET_H
#define EGAMEWIDGET_H

#include "../elabel.h"

#include "../../egameinput.h"
#include "../../egameworld.h"
#include "../../emaincharaction.h"
#include "../../textures/echarunitmodel.h"
#include "../../textures/etilesiterator.h"
#include "../../units/eunit.h"
#include "egamepainter.h"
#include "egrounditemnames.h"

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/epathfinder.h>
#include <eSlayerHelpers/epoint.h>
#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerServer/eserver.h>
#include <eSlayerHelpers/emissile.h>
#include <eSlayerHelpers/earea.h>
#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/ehireinfo.h>

class eUnitIndicator;
class eCharacter;

using eDeathHandler = std::function<void()>;
using eRespawnHandler = std::function<void()>;

struct eScreenMessage {
    std::string fText;
    std::shared_ptr<eTexture> fTex;
    int fFramesRemaining;
};

using eMoveToMapAction = std::function<void(const eMoveToMapData& moveData)>;

class eGameWidget : public eLabel {
public:
    eGameWidget(eMainWindow* const window);
    ~eGameWidget();

    void initialize(const uint32_t clientId,
                    const std::shared_ptr<eServer>& server,
                    const std::shared_ptr<eMap>& map,
                    const eCharacter& c,
                    const eTeamId teamId,
                    const eMoveToMapAction& move);

    int tileWidth() const { return mInput.tileWidth(); }
    int tileHeight() const { return mInput.tileHeight(); }

    const std::shared_ptr<eMap>& map() const { return mMap; }

    int mapWidth() const { return mMap->width(); }
    int mapHeight() const { return mMap->height(); }

    const ePointF& characterPos() const;

    ePointF pixelToTilePos(const ePointF& pos,
                           const ePointF& pixel) const;
    ePointF pixelToTilePos(const ePointF& pixel) const;
    ePointF tilePosToPixel(const ePointF& pos) const;
    ePoint tilePosToIPixel(const ePointF& pos) const;

    void setUnitIndicator(eUnitIndicator* const indicator);

    void setMenuVisible(const bool v) { mMenuVisible = v; }

    void stop();
    eMainCharAction& mainAction() const { return *mMainAction; }
    eUnit& mainChar() const { return *mMainChar; }

    eCharacter character();

    eGameInput& input() { return mInput; }
    const eGameInput& input() const { return mInput; }

    const std::string& cname() const { return mCName; }
    eEquipment& equipment() { return mMainAction->equipment(); }
    eAttributes& attributes() { return mMainAction->attributes(); }
    eSlayerQuests& quests() { return mMainAction->quests(); }
    eTalkHeard& talkHeard() { return mMainAction->talkHeard(); }
    std::optional<eMercenary>& merc() { return mMainAction->merc(); }
    eStats& stats() { return mMainAction->stats(); }
    eTeamId team() const { return mMainChar->fTeamId; }
    bool running() const { return mMainAction->running(); }
    void dropItem();
    void dropGold(const int count);
    void sendEqAction(const eEquipmentAction& a);
    void sendBuyAction(const eBuyAction& a);
    void sendSellAction(const eSellAction& a);
    void sendAttributesChanged();
    void sendSkillLevelsChanged();

    void setLeftSkill(const int s);
    void setRightSkill(const int s);

    void setOtherLeftSkill(const int s);
    void setOtherRightSkill(const int s);

    const std::shared_ptr<eServer>& server() const { return mServer; }
    uint32_t clientId() const { return mClientId; }

    static const std::vector<std::string>& messageLog()
    { return sMessageLog; }
    static void clearMessageLog()
    { sMessageLog.clear(); }

    void setDeathHandler(const eDeathHandler& h) { mDeathHandler = h; }
    void setRespawnHandler(const eRespawnHandler& h) { mRespawnHandler = h; }

    void respawn();
    bool switchRunning();
    bool switchWeapons();
    void disconnect();
    void save();
    void sendMessage(const std::string& text);
    void consumePotion(const int x);
    void consumePotion(const eItem& p);

    void waypointTeleport(const eAreaIds& area);
    void spawnPortal();
    void openSellerMenu(const uint32_t sellerId);
    void addSocket(const uint8_t questId);

    bool hire(const eHireInfo& info);

    void updateWantsToTalk();

    static eGameWidget* sInstance;
    static void sSendEqAction(const eEquipmentAction& a);
    static void sSendBuyAction(const eBuyAction& a);
    static void sSendSellAction(const eSellAction& a);
    static void sSendSkillLevelsChanged();
    static void sSendAttributesChanged();
    static void sMoveToMap(const eMoveToMapData& moveData);
    static void sDropGold(const int gold);
    static void sOpenSellerMenu(const uint32_t sellerId);
    static void sAddSocket(const uint8_t questId);
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
private:
    void initializeTextures();
    void setHighlightedUnit(const std::shared_ptr<eUnit>& u);
    void setIndicatorUnit(const std::shared_ptr<eUnit>& u);
    void setHighlightedObject(const std::shared_ptr<eObject>& obj);
    void setHighlightedDoors(const std::optional<eDoors>& doors);
    void setHighlightedStairs(const std::optional<eStairs>& stairs);
    void setHighlightedItem(const std::shared_ptr<eGroundItem>& i);
    void setPressedUnit(const std::shared_ptr<eUnit>& u);

    void addMessage(SDL_Renderer* const r,
                    const std::string& msg);

    static std::vector<std::string> sMessageLog;

    std::string mCName;
    bool mHardcore;

    eGameWorld mWorld;
    eGameInput mInput;
    eGamePainter mGamePainter;

    uint32_t mClientId = 0;
    std::shared_ptr<eServer> mServer;

    std::shared_ptr<eMainCharAction> mMainAction;
    std::shared_ptr<eUnit> mMainChar;

    std::vector<eScreenMessage> mMessages;

    std::weak_ptr<eUnit> mHighlightUnit;
    std::weak_ptr<eUnit> mPressedUnit;

    std::weak_ptr<eObject> mHighlightObject;

    std::weak_ptr<eGroundItem> mHighlightItem;

    std::optional<eDoors> mHighlightDoors;
    std::optional<eStairs> mHighlightStairs;

    std::shared_ptr<eMap> mMap;
    eTilesIterator mTileIterator;

    eGroundItemNames mItemNames;

    int mFrame = 0;

    bool mMenuVisible = false;

    eUnitIndicator* mUnitIndicator = nullptr;

    int mLeftSkill = 0;
    int mRightSkill = 0;

    int mOtherLeftSkill = 0;
    int mOtherRightSkill = 0;

    eDeathHandler mDeathHandler;
    eRespawnHandler mRespawnHandler;

    int mLastArea = -1;

    eMoveToMapAction mMoveAction;

    std::set<uint32_t> mPortals;
    uint16_t mPortalsVersion = 0;
};

#endif // EGAMEWIDGET_H

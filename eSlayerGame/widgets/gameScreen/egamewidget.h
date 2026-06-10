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

class eUnitIndicator;
class eCharacter;

using eDeathHandler = std::function<void()>;
using eRespawnHandler = std::function<void()>;

struct eScreenMessage {
    std::string fText;
    std::shared_ptr<eTexture> fTex;
    int fFramesRemaining;
};

using eMoveToMapAction = std::function<void(const uint8_t mapId)>;

class eGameWidget : public eLabel {
public:
    eGameWidget(eMainWindow* const window);
    ~eGameWidget();

    void initialize(const int clientId,
                    const std::shared_ptr<eServer>& server,
                    const std::shared_ptr<eMap>& map,
                    const eCharacter& c,
                    const eTeamId teamId,
                    const eMoveToMapAction& move);

    int tileWidth() const { return mInput.tileWidth(); }
    int tileHeight() const { return mInput.tileHeight(); }

    int mapWidth() const { return mMap->width(); }
    int mapHeight() const { return mMap->height(); }

    const ePointF& characterPos() const;

    ePointF pixelToTilePos(const ePointF& pos,
                           const ePointF& pixel) const;
    ePointF pixelToTilePos(const ePointF& pixel) const;
    ePointF tilePosToPixel(const ePointF& pos) const;

    void setUnitIndicator(eUnitIndicator* const indicator);

    void setMenuVisible(const bool v) { mMenuVisible = v; }

    void stop();
    eMainCharAction& mainAction() { return *mMainAction; }

    eCharacter character();

    eGameInput& input() { return mInput; }
    const eGameInput& input() const { return mInput; }

    const std::string& cname() const { return mCName; }
    std::string userName(const int clientId) const;
    eEquipment& equipment() { return mMainAction->equipment(); }
    std::vector<eEquipment>& bodyEquipments() { return mMainAction->bodyEquipments(); }
    eAttributes& attributes() { return mMainAction->attributes(); }
    eStats& stats() { return mMainAction->stats(); }
    eTeamId team() const { return mMainChar->fTeamId; }
    void dropItem();
    void sendInventoryRearranged();
    void sendAttributesChanged();
    void sendSkillLevelsChanged();

    void setLeftSkill(const int s);
    void setRightSkill(const int s);

    void setOtherLeftSkill(const int s);
    void setOtherRightSkill(const int s);

    const std::shared_ptr<eServer>& server() const { return mServer; }
    int clientId() const { return mClientId; }

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

    static eGameWidget* sInstance;
    static void sSendInventoryRearranged();
    static void sSendSkillLevelsChanged();
    static void sSendAttributesChanged();
    static void sMoveToMap(const uint8_t mapId);
    static void sClearAll();
    static std::map<int, std::string> sUserNames;
protected:
    void paintEvent(ePainter& p) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
private:
    void initializeTextures();
    void setHighlightedUnit(const std::shared_ptr<eUnit>& u);
    void setHighlightedObject(const std::shared_ptr<eObject>& obj);
    void setHighlightedDoors(const std::optional<eDoors>& doors);
    void setHighlightedStairs(const std::optional<eStairs>& stairs);
    void setHighlightedItem(const std::shared_ptr<eGroundItem>& i);
    void setPressedUnit(const std::shared_ptr<eUnit>& u);

    void addMessage(SDL_Renderer* const r,
                    const std::string& msg);

    std::string mCName;
    bool mHardcore;

    eGameWorld mWorld;
    eGameInput mInput;
    eGamePainter mGamePainter;

    int mClientId = -1;
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

    std::vector<eBody> mBodies;
};

#endif // EGAMEWIDGET_H

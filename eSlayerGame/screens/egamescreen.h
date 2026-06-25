#ifndef EGAMESCREEN_H
#define EGAMESCREEN_H

#include "escreenbase.h"

#include <eSlayerServer/eserver.h>
#include <eSlayerMapGenerator/emapgenerator.h>

class eESCMenu;
class eGameWidget;
class eUnitIndicator;
class ePlayerHealthIndicator;
class eSkillButton;
class eSkillTreesWidget;
class eTextureCheckButton;
class eInventoryWidget;
class eInventoryBagpackWidget;
class eHoverWidget;
class eMiniMap;
class eStatsWidget;
class eCharacter;
class eLineEdit;
class eBgWidget;
class ePartyWidget;
class eWaypointWidget;
struct eMoveToMapData;
class eBottomWidget;
class eStashWidget;
struct eSeller;
class eSellerWidget;

class eGameScreen : public eScreenBase {
public:
    eGameScreen(eMainWindow* const window);

    using eMoveToMapAction = std::function<void(const eMoveToMapData& moveData)>;
    void setExitAction(const eAction& a);
    void initialize(const uint32_t clientId,
                    const std::shared_ptr<eServer>& server,
                    const std::shared_ptr<eMap>& map,
                    const eCharacter& c,
                    const eTeamId teamId,
                    const eMoveToMapAction& move);

    static eGameScreen* sInstance;
    static void sOpenWaypointMenu(
        const uint8_t actId,
        const uint8_t mapId,
        const uint8_t areaId);
    static void sOpenStash();
    static void sOpenSellerMenu(
        const eSeller& s);
    static void sCloseObjectMenu();
protected:
    bool keyPressEvent(const eKeyPressEvent& e) override;
    void paintEvent(ePainter&) override;
private:
    void setLeftSkill(const int skillId);
    void setRightSkill(const int skillId);

    void hotkeyPressed(const int fkey);
    void consumePotion(const int x);

    void hidePositionedMenu();

    void hideLeftMenu();
    void hideRightMenu();

    void showDeadMenu();

    void showESCMenu();
    void hideESCMenu();

    void showInventoryMenu(
        const eHoverItemType htype = eHoverItemType::regular);
    void hideInventoryMenu();

    void showPartyMenu();
    void hidePartyMenu();

    void showStatsMenu();
    void hideStatsMenu();

    void showSkillTreesMenu();
    void hideSkillTreesMenu();

    void showMessageBox();
    void hideMessageBox();

    void showWaypointMenu(const uint8_t cActId,
                          const uint8_t cMapId,
                          const uint8_t cAreaId);
    void hideWaypointMenu();

    void showSellerMenu(const eSeller& seller);
    void hideSellerMenu();

    void showStashMenu();
    void hideStashMenu();

    void hideInventoryConnectedMenu();

    void openSkillMenu(const eAlignment align,
                       eSkillButton* const targetButton,
                       int& targetSkillVar,
                       const eSkillChoice schoice);
    void updateCharPos();

    eGameWidget* mGameWidget = nullptr;

    eAction mExitAction;
    eESCMenu* mESCMenu = nullptr;
    eWidget* mDeadMenu = nullptr;
    eInventoryWidget* mInventoryMenu = nullptr;
    ePartyWidget* mPartyMenu = nullptr;
    eWaypointWidget* mWaypointMenu = nullptr;
    eSellerWidget* mSellerMenu = nullptr;
    eStatsWidget* mStatsMenu = nullptr;
    eSkillTreesWidget* mSkillTreesMenu = nullptr;
    eHoverWidget* mDragWidget = nullptr;
    eMiniMap* mMiniMap = nullptr;
    eStashWidget* mStashMenu = nullptr;
    eUnitIndicator* mUnitIndicator = nullptr;

    eWidget* mSkillMenu = nullptr;

    eBottomWidget* mBottomWidget = nullptr;
    eWidget* mMenusWidget = nullptr;

    int mLeftSkill = 0;
    int mRightSkill = 0;

    int mOtherLeftSkill = 0;
    int mOtherRightSkill = 0;

    eLineEdit* mMessage = nullptr;
};

#endif // EGAMESCREEN_H

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

class eGameScreen : public eScreenBase {
public:
    eGameScreen(eMainWindow* const window);

    using eMoveToMapAction = std::function<void(const uint8_t mapId)>;
    void setExitAction(const eAction& a);
    void initialize(const int clientId,
                    const std::shared_ptr<eServer>& server,
                    const std::shared_ptr<eMap>& map,
                    const eCharacter& c,
                    const eTeamId teamId,
                    const eMoveToMapAction& move);
protected:
    bool keyPressEvent(const eKeyPressEvent& e) override;
    void paintEvent(ePainter&) override;
private:
    void setLeftSkill(const int skillId);
    void setRightSkill(const int skillId);

    void hotkeyPressed(const int fkey);
    void consumePotion(const int x);

    void hideLeftMenu();
    void hideRightMenu();

    void showDeadMenu();

    void showESCMenu();
    void hideESCMenu();

    void showInventoryMenu();
    void hideInventoryMenu();

    void showPartyMenu();
    void hidePartyMenu();

    void showStatsMenu();
    void hideStatsMenu();

    void showSkillTreesMenu();
    void hideSkillTreesMenu();

    void showMessageBox();
    void hideMessageBox();

    void hideBeltExt();
    void showBeltExt();

    void openSkillMenu(const eAlignment align,
                       eSkillButton* const targetButton,
                       int& targetSkillVar,
                       const eSkillChoice schoice);
    void updateCharPos();

    eGameWidget* mGameWidget = nullptr;

    eAction mExitAction;
    eESCMenu* mESCMenu = nullptr;
    eWidget* mDeadMenu = nullptr;
    eBgWidget* mBottomWid = nullptr;
    eInventoryWidget* mInventoryMenu = nullptr;
    ePartyWidget* mPartyMenu = nullptr;
    eInventoryBagpackWidget* mBelt = nullptr;
    bool mBeltExtTmp = false;
    eInventoryBagpackWidget* mBeltExt = nullptr;
    eStatsWidget* mStatsMenu = nullptr;
    eSkillTreesWidget* mSkillTreesMenu = nullptr;
    eHoverWidget* mDragWidget = nullptr;
    eMiniMap* mMiniMap = nullptr;
    eUnitIndicator* mUnitIndicator = nullptr;
    ePlayerHealthIndicator* mHealthIndicator = nullptr;
    ePlayerHealthIndicator* mManaIndicator = nullptr;
    ePlayerHealthIndicator* mStaminaIndicator = nullptr;
    ePlayerHealthIndicator* mExperienceIndicator = nullptr;

    eSkillButton* mLeftSkillButton = nullptr;
    eSkillButton* mRightSkillButton = nullptr;
    eWidget* mSkillMenu = nullptr;

    int mLeftSkill = 0;
    int mRightSkill = 0;

    int mOtherLeftSkill = 0;
    int mOtherRightSkill = 0;

    eTextureCheckButton* mRunButton = nullptr;

    eLineEdit* mMessage = nullptr;
};

#endif // EGAMESCREEN_H

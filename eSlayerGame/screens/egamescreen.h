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

class eGameScreen : public eScreenBase {
public:
    eGameScreen(eMainWindow* const window);
    ~eGameScreen();

    void setExitAction(const eAction& a);
    void initialize(const int clientId,
                    const std::shared_ptr<eServer>& server,
                    const std::shared_ptr<eMap>& map,
                    const eCharacter& c,
                    const eTeamId teamId);
protected:
    bool keyPressEvent(const eKeyPressEvent& e) override;
    void paintEvent(ePainter&) override;
private:
    void hotkeyPressed(const int fkey);
    void consumePotion(const int x);

    void showDeadMenu();

    void showESCMenu();
    void hideESCMenu();

    void showInventoryMenu();
    void hideInventoryMenu();

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
    eTextureCheckButton* mRunButton = nullptr;

    eLineEdit* mMessage = nullptr;
};

#endif // EGAMESCREEN_H

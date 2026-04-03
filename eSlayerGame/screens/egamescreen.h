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
class eTextureCheckButton;
class eInventoryWidget;
class eItemDragWidget;
class eStatsWidget;
class eCharacter;
class eLineEdit;

class eGameScreen : public eScreenBase {
public:
    eGameScreen(eMainWindow* const window);
    ~eGameScreen();

    void setExitAction(const eAction& a);
    void initialize(const int clientId,
                    const std::shared_ptr<eServer>& server,
                    const std::shared_ptr<eMap>& map,
                    const eCharacter& c);
protected:
    bool keyPressEvent(const eKeyPressEvent& e) override;
    void paintEvent(ePainter&) override;
private:
    void showDeadMenu();

    void showESCMenu();
    void hideESCMenu();

    void showInventoryMenu();
    void hideInventoryMenu();

    void showStatsMenu();
    void hideStatsMenu();

    void openSkillMenu(const eAlignment align,
                       eSkillButton* const targetButton,
                       int& targetSkillVar);
    void updateCharPos();

    eGameWidget* mGameWidget = nullptr;

    eAction mExitAction;
    eESCMenu* mESCMenu = nullptr;
    eWidget* mDeadMenu = nullptr;
    eWidget* mBottomWid = nullptr;
    eInventoryWidget* mInventoryMenu = nullptr;
    eStatsWidget* mStatsMenu = nullptr;
    eItemDragWidget* mDragWidget = nullptr;
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

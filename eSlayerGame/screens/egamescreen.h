#ifndef EGAMESCREEN_H
#define EGAMESCREEN_H

#include "escreenbase.h"

#include <eSlayerServer/eserver.h>
#include <eSlayerMapGenerator/emapgenerator.h>

class eGameWidget;
class eUnitIndicator;
class ePlayerHealthIndicator;
class eSkillButton;
class eCheckButton;

class eGameScreen : public eScreenBase {
public:
    eGameScreen(eMainWindow* const window);
    ~eGameScreen();

    void setExitAction(const eAction& a);
    void initialize(const int clientId,
                    const std::shared_ptr<eServer>& server,
                    const std::shared_ptr<eMap>& map);
protected:
    bool keyPressEvent(const eKeyPressEvent& e) override;
private:
    void showDeadMenu();
    void showESCMenu();
    void hideESCMenu();
    void openSkillMenu(const eAlignment align,
                       eSkillButton* const targetButton,
                       int& targetSkillVar);

    eGameWidget* mGameWidget = nullptr;

    eAction mExitAction;
    eWidget* mESCMenu = nullptr;
    eWidget* mDeadMenu = nullptr;
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
    eCheckButton* mRunButton = nullptr;
};

#endif // EGAMESCREEN_H

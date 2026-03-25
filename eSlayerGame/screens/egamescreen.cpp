#include "egamescreen.h"

#include "../elanguage.h"
#include "../emainwindow.h"
#include "../textures/euitextures.h"
#include "../widgets/echeckbutton.h"
#include "../widgets/ecolors.h"
#include "../widgets/gameScreen/eescmenubutton.h"
#include "../widgets/gameScreen/egamewidget.h"
#include "../widgets/gameScreen/eplayerhealthindicator.h"
#include "../widgets/gameScreen/eunitindicator.h"
#include "../widgets/gameScreen/eskillbutton.h"
#include "../widgets/gameScreen/eskillselectwidget.h"

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eunitdata.h>

eGameScreen::eGameScreen(eMainWindow* const window) :
    eScreenBase(window) {}

eGameScreen::~eGameScreen() {
    if(mGameWidget) {
        const auto server = mGameWidget->server();
        if(server) {
            server->disconnect(mGameWidget->clientId());
        }
    }
}

void eGameScreen::setExitAction(const eAction& a) {
    mExitAction = a;
}

void eGameScreen::initialize(const int clientId,
                             const std::shared_ptr<eServer>& server,
                             const std::shared_ptr<eMap>& map) {
    mGameWidget = new eGameWidget(window());
    mGameWidget->resize(width(), height());
    addWidget(mGameWidget);

    mGameWidget->initialize(clientId, server, map);

    mGameWidget->setMainCharHandler([this](const eUnitData& u) {
        mHealthIndicator->setValue(u.fHealth);
        mHealthIndicator->setRange(0, u.fMaxHealth);
        mStaminaIndicator->setValue(mGameWidget->mainAction().stamina());
        mStaminaIndicator->setRange(0, mGameWidget->mainAction().maxStamina());
    });

    mGameWidget->setDeathHandler([this]() {
        if(!mDeadMenu) showDeadMenu();
    });

    mGameWidget->setRespawnHandler([this]() {
        if(mDeadMenu) {
            mDeadMenu->deleteLater();
            mDeadMenu = nullptr;
        }
    });

    mUnitIndicator = new eUnitIndicator(window());
    mUnitIndicator->initialize();
    const float m = resolution().multiplier();
    mUnitIndicator->resize(200*m, 40*m);
    addWidget(mUnitIndicator);
    mUnitIndicator->align(eAlignment::hcenter | eAlignment::top);
    mUnitIndicator->setY(20*m);

    mGameWidget->setUnitIndicator(mUnitIndicator);

    const int indicatorW = 400*m;
    const int indicatorH = 30*m;

    const auto bottomWid = new eWidget(window());
    bottomWid->setNoPadding();

    mLeftSkillButton = new eSkillButton(window());
    mLeftSkillButton->initialize();
    mLeftSkillButton->setPressAction([this]() {
        openSkillMenu(eAlignment::left, mLeftSkillButton, mLeftSkill);
    });
    bottomWid->addWidget(mLeftSkillButton);

    const auto centerWid = new eWidget(window());
    centerWid->setNoPadding();

    mExperienceIndicator = new ePlayerHealthIndicator(window());
    mExperienceIndicator->setColor(eColors::sExperience);
    mExperienceIndicator->setName(eLanguage::text(7, 3));
    mExperienceIndicator->initialize();
    centerWid->addWidget(mExperienceIndicator);
    mExperienceIndicator->resize(2*indicatorW, indicatorH/2);

    const auto healthMana = new eWidget(window());
    healthMana->setNoPadding();

    mHealthIndicator = new ePlayerHealthIndicator(window());
    mHealthIndicator->setColor(eColors::sHealth);
    mHealthIndicator->setName(eLanguage::text(7, 0));
    mHealthIndicator->initialize();
    healthMana->addWidget(mHealthIndicator);
    mHealthIndicator->resize(indicatorW, indicatorH);

    mManaIndicator = new ePlayerHealthIndicator(window());
    mManaIndicator->setColor(eColors::sMana);
    mManaIndicator->setName(eLanguage::text(7, 1));
    mManaIndicator->initialize();
    healthMana->addWidget(mManaIndicator);
    mManaIndicator->resize(indicatorW, indicatorH);

    const int lineWidth = eLabel::lineWidth();
    healthMana->stackHorizontally(-lineWidth);
    healthMana->fitContent();
    centerWid->addWidget(healthMana);

    const auto staminaExperience = new eWidget(window());
    staminaExperience->setNoPadding();

    const auto staminaWid = new eWidget(window());
    staminaWid->setNoPadding();

    mRunButton = new eCheckButton(window());
    mRunButton->setNoPadding();
    mRunButton->setCheckAction([this](const bool check) {
        if(check) mRunButton->setTexture(eUITextures::sRunIcon);
        else mRunButton->setTexture(eUITextures::sWalkIcon);
        mGameWidget->mainAction().setRunning(check);
    });
    mRunButton->setTexture(eUITextures::sWalkIcon);
    mRunButton->fitContent();
    staminaWid->addWidget(mRunButton);

    mStaminaIndicator = new ePlayerHealthIndicator(window());
    mStaminaIndicator->setColor(eColors::sStamina);
    mStaminaIndicator->setName(eLanguage::text(7, 2));
    mStaminaIndicator->initialize();
    staminaWid->addWidget(mStaminaIndicator);
    mStaminaIndicator->resize(indicatorW - mRunButton->width(),
                              mRunButton->height());

    staminaWid->stackHorizontally();
    staminaWid->fitContent();

    staminaExperience->addWidget(staminaWid);

    staminaExperience->stackHorizontally(-lineWidth);
    staminaExperience->fitContent();
    centerWid->addWidget(staminaExperience);

    centerWid->stackVertically(-lineWidth);
    centerWid->fitContent();
    centerWid->align(eAlignment::bottom | eAlignment::hcenter);
    bottomWid->addWidget(centerWid);

    mRightSkillButton = new eSkillButton(window());
    mRightSkillButton->initialize();
    mRightSkillButton->setPressAction([this]() {
        openSkillMenu(eAlignment::right, mRightSkillButton, mRightSkill);
    });
    bottomWid->addWidget(mRightSkillButton);

    bottomWid->stackHorizontally();
    bottomWid->fitContent();
    addWidget(bottomWid);
    bottomWid->align(eAlignment::bottom | eAlignment::hcenter);
}

bool eGameScreen::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_ESCAPE) {
        if(mSkillMenu) {
            mSkillMenu->deleteLater();
            mSkillMenu = nullptr;
        } else if(mDeadMenu) {
            mGameWidget->server()->respawn(mGameWidget->clientId());
        } else {
            if(mESCMenu) {
                hideESCMenu();
            } else {
                showESCMenu();
            }
        }
    } else if(e.key() == SDL_SCANCODE_R) {
        const bool run = !mGameWidget->mainAction().running();
        mRunButton->setChecked(run);
        mGameWidget->mainAction().setRunning(run);
    }
    return true;
}

void eGameScreen::showDeadMenu() {
    mDeadMenu = new eWidget(window());

    const auto line1 = new eLabel(window());
    line1->setExtraHugeFontSize();
    line1->setFontColor(eFontColor::redBlack);
    line1->setText(eLanguage::text(5, 3));
    line1->fitContent();
    mDeadMenu->addWidget(line1);

    const auto line2 = new eLabel(window());
    line2->setExtraHugeFontSize();
    line2->setFontColor(eFontColor::redBlack);
    line2->setText(eLanguage::text(5, 4));
    line2->fitContent();
    mDeadMenu->addWidget(line2);

    const auto res = resolution();
    const int p = res.hugePadding();
    mDeadMenu->stackVertically(p);
    mDeadMenu->fitContent();

    line1->align(eAlignment::hcenter);
    line2->align(eAlignment::hcenter);

    addWidget(mDeadMenu);
    mDeadMenu->align(eAlignment::center);
}

void eGameScreen::showESCMenu() {
    mESCMenu = new eWidget(window());

    const auto optionsB = new eESCMenuButton(
        eLanguage::text(5, 0), window());
    mESCMenu->addWidget(optionsB);

    const auto exitB = new eESCMenuButton(
        eLanguage::text(5, 1), window());
    mESCMenu->addWidget(exitB);
    exitB->setPressAction([this]() {
        const auto server = mGameWidget->server();
        if(server) {
            server->disconnect(mGameWidget->clientId());
        }
        mExitAction();
    });

    const auto returnB = new eESCMenuButton(
        eLanguage::text(5, 2), window());
    mESCMenu->addWidget(returnB);
    returnB->setPressAction([this]() {
        hideESCMenu();
    });

    const auto res = resolution();
    const int p = res.hugePadding();
    mESCMenu->stackVertically(p);
    mESCMenu->fitContent();

    optionsB->align(eAlignment::hcenter);
    exitB->align(eAlignment::hcenter);
    returnB->align(eAlignment::hcenter);

    addWidget(mESCMenu);
    mESCMenu->align(eAlignment::center);

    mGameWidget->setMenuVisible(true);
}

void eGameScreen::hideESCMenu() {
    if(!mESCMenu) return;
    mESCMenu->deleteLater();
    mESCMenu = nullptr;
    mGameWidget->setMenuVisible(false);
}

void eGameScreen::openSkillMenu(const eAlignment align,
                                eSkillButton* const targetButton,
                                int& targetSkillVar) {
    if(mSkillMenu) {
        mSkillMenu->deleteLater();
        mSkillMenu = nullptr;
        return;
    }

    const auto w = new eSkillSelectWidget(window());

    std::vector<int> skillIds;
    for(const auto& s : eSkills::sSkills) {
        skillIds.push_back(s.fId);
    }

    const auto action = [this, targetButton, &targetSkillVar](const int skillId) {
        targetButton->setSkillId(skillId);
        targetSkillVar = skillId;
        mGameWidget->setLeftSkill(mLeftSkill);
        mGameWidget->setRightSkill(mRightSkill);
        mSkillMenu = nullptr;
    };
    w->initialize(skillIds, align, action);

    addWidget(w);

    const auto res = resolution();
    const float mult = res.multiplier();
    const int margin = 100*mult;
    w->move(align == eAlignment::left ? margin : width() - w->width() - margin,
            height() - w->height() - margin);

    mSkillMenu = w;
}

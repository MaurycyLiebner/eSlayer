#include "egamescreen.h"

#include "../elanguage.h"
#include "../emainwindow.h"
#include "../textures/euitextures.h"
#include "../widgets/etexturecheckbutton.h"
#include "../widgets/ecolors.h"
#include "../widgets/gameScreen/eescmenu.h"
#include "../widgets/gameScreen/egamewidget.h"
#include "../widgets/gameScreen/eplayerhealthindicator.h"
#include "../widgets/gameScreen/eunitindicator.h"
#include "../widgets/gameScreen/eskillbutton.h"
#include "../widgets/gameScreen/eskillselectwidget.h"
#include "../widgets/gameScreen/einventorywidget.h"
#include "../widgets/gameScreen/eitemdragwidget.h"
#include "../widgets/gameScreen/estatswidget.h"

#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eunitdata.h>

eGameScreen::eGameScreen(eMainWindow* const window) :
    eScreenBase(window) {}

eGameScreen::~eGameScreen() {
    if(mGameWidget) {
        mGameWidget->disconnect();
    }
}

void eGameScreen::setExitAction(const eAction& a) {
    mExitAction = a;
}

void eGameScreen::initialize(const int clientId,
                             const std::shared_ptr<eServer>& server,
                             const std::shared_ptr<eMap>& map,
                             const eEquipment& eq) {
    mGameWidget = new eGameWidget(window());
    mGameWidget->resize(width(), height());
    addWidget(mGameWidget);

    mGameWidget->initialize(clientId, server, map, eq);

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

    mBottomWid = new eWidget(window());
    mBottomWid->setNoPadding();

    mLeftSkillButton = new eSkillButton(window());
    mLeftSkillButton->initialize();
    mLeftSkillButton->setPressAction([this]() {
        openSkillMenu(eAlignment::left, mLeftSkillButton, mLeftSkill);
    });
    mBottomWid->addWidget(mLeftSkillButton);

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

    mRunButton = new eTextureCheckButton(window());
    mRunButton->setCheckAction([this](const bool check) {
        auto& action = mGameWidget->mainAction();
        action.setRunning(check);
    });
    mRunButton->initialize(eUITextures::sRunIcon,
                           eUITextures::sWalkIcon);
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
    mBottomWid->addWidget(centerWid);

    mRightSkillButton = new eSkillButton(window());
    mRightSkillButton->initialize();
    mRightSkillButton->setPressAction([this]() {
        openSkillMenu(eAlignment::right, mRightSkillButton, mRightSkill);
    });
    mBottomWid->addWidget(mRightSkillButton);

    mBottomWid->stackHorizontally();
    mBottomWid->fitContent();
    addWidget(mBottomWid);
    mBottomWid->align(eAlignment::bottom | eAlignment::hcenter);
}

bool eGameScreen::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_ESCAPE) {
        if(mInventoryMenu) {
            hideInventoryMenu();
        } else if(mSkillMenu) {
            mSkillMenu->deleteLater();
            mSkillMenu = nullptr;
        } else if(mDeadMenu) {
            mGameWidget->respawn();
        } else {
            if(mESCMenu) {
                hideESCMenu();
            } else {
                showESCMenu();
            }
        }
    } else if(e.key() == SDL_SCANCODE_R) {
        const bool run = mGameWidget->switchRunning();
        mRunButton->setChecked(run);
    } else if(e.key() == SDL_SCANCODE_I) {
        if(mInventoryMenu) {
            hideInventoryMenu();
        } else {
            showInventoryMenu();
        }
    } else if(e.key() == SDL_SCANCODE_W) {
        mGameWidget->switchWeapons();
        if(mInventoryMenu) {
            mInventoryMenu->updateWeapons();
        }
    } else if(e.key() == SDL_SCANCODE_A) {
        if(mStatsMenu) {
            hideStatsMenu();
        } else {
            showStatsMenu();
        }
    }
    return true;
}

void eGameScreen::paintEvent(ePainter&) {
    const auto& stats = mGameWidget->stats();
    const auto& attrs = mGameWidget->attributes();

    mHealthIndicator->setRange(0, stats.fMaxHealth);
    mHealthIndicator->setValue(stats.fHealthF);

    mManaIndicator->setRange(0, stats.fMaxMana);
    mManaIndicator->setValue(stats.fManaF);

    const auto& action = mGameWidget->mainAction();
    mStaminaIndicator->setRange(0, action.maxStamina());
    mStaminaIndicator->setValue(action.stamina());

    mExperienceIndicator->setRange(0, attrs.nextLevelExp());
    mExperienceIndicator->setValue(stats.fExperience);
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
    if(mESCMenu) return;
    mGameWidget->stop();
    mESCMenu = new eESCMenu(window());
    const auto return_ = [this]() {
        hideESCMenu();
    };
    const auto exit = [this]() {
        mGameWidget->disconnect();
        mExitAction();
    };
    mESCMenu->initialize(return_, exit);

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

void eGameScreen::showInventoryMenu() {
    if(mInventoryMenu) return;
    mInventoryMenu = new eInventoryWidget(window());
    const int w = width();
    const int h = height();
    mInventoryMenu->resize(w/2, h - mBottomWid->height());
    auto& eq = mGameWidget->equipment();
    mInventoryMenu->initialize(&eq);
    addWidget(mInventoryMenu);
    mInventoryMenu->align(eAlignment::right | eAlignment::top);

    mDragWidget = new eItemDragWidget(window());
    mDragWidget->resize(w, h);
    mDragWidget->initialize([this](SDL_Point pos) {
        pos.x -= mInventoryMenu->x();
        pos.y -= mInventoryMenu->y();
        if(pos.x < 0) {
            mGameWidget->dropItem();
        } else {
            const bool r = mInventoryMenu->dropItem(pos);
        }
    });
    addWidget(mDragWidget);
    eItemDragWidget::sUpdateDragItem(eq);

    updateCharPos();
}

void eGameScreen::hideInventoryMenu() {
    if(!mInventoryMenu) return;
    mInventoryMenu->deleteLater();
    mInventoryMenu = nullptr;
    mDragWidget->deleteLater();
    mDragWidget = nullptr;
    updateCharPos();
}

void eGameScreen::showStatsMenu() {
    if(mStatsMenu) return;
    mStatsMenu = new eStatsWidget(window());
    const int w = width();
    const int h = height();
    mStatsMenu->resize(w/2, h - mBottomWid->height());
    const auto& stats = mGameWidget->stats();
    mStatsMenu->initialize(&stats);
    addWidget(mStatsMenu);
    mStatsMenu->align(eAlignment::left | eAlignment::top);
    updateCharPos();
}

void eGameScreen::hideStatsMenu() {
    if(!mStatsMenu) return;
    mStatsMenu->deleteLater();
    mStatsMenu = nullptr;
    updateCharPos();
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

void eGameScreen::updateCharPos() {
    auto& input = mGameWidget->input();
    if(mStatsMenu && mInventoryMenu) {
        input.setCharacterHorizontalPos(0.5f);
    } else if(mStatsMenu) {
        input.setCharacterHorizontalPos(0.75f);
    } else if(mInventoryMenu) {
        input.setCharacterHorizontalPos(0.25f);
    } else {
        input.setCharacterHorizontalPos(0.5f);
    }
}

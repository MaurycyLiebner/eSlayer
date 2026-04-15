#include "egamescreen.h"

#include "../elanguage.h"
#include "../emainwindow.h"
#include "../textures/euitextures.h"
#include "../widgets/ecolors.h"
#include "../widgets/etexturecheckbutton.h"
#include "../widgets/gameScreen/eescmenu.h"
#include "../widgets/gameScreen/egamewidget.h"
#include "../widgets/gameScreen/einventorywidget.h"
#include "../widgets/gameScreen/eitemdragwidget.h"
#include "../widgets/gameScreen/eplayerhealthindicator.h"
#include "../widgets/gameScreen/eskillbutton.h"
#include "../widgets/gameScreen/eskillselectwidget.h"
#include "../widgets/gameScreen/estatswidget.h"
#include "../widgets/gameScreen/eskilltreeswidget.h"
#include "../widgets/gameScreen/eunitindicator.h"
#include "../widgets/gameScreen/ebgwidget.h"
#include "../widgets/elineedit.h"

#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/echaracter.h>
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
                             const eCharacter& c) {
    mGameWidget = new eGameWidget(window());
    mGameWidget->resize(width(), height());
    addWidget(mGameWidget);

    mGameWidget->initialize(clientId, server, map, c);

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
    const auto& res = resolution();
    const float m = res.multiplier();
    mUnitIndicator->resize(200*m, 40*m);
    addWidget(mUnitIndicator);
    mUnitIndicator->align(eAlignment::hcenter | eAlignment::top);
    mUnitIndicator->setY(20*m);

    mGameWidget->setUnitIndicator(mUnitIndicator);

    mBottomWid = new eBgWidget(window());
    mBottomWid->setHugePadding();

    const auto bottomInnerWidget = new eWidget(window());
    bottomInnerWidget->setNoPadding();

    mLeftSkillButton = new eSkillButton(window());
    mLeftSkillButton->initialize(static_cast<int>(eSkillChoice::left));
    mLeftSkillButton->setPressAction([this]() {
        openSkillMenu(eAlignment::left, mLeftSkillButton, mLeftSkill,
                      eSkillChoice::left);
    });
    bottomInnerWidget->addWidget(mLeftSkillButton);

    const auto centerWid = new eWidget(window());
    centerWid->setNoPadding();

    mExperienceIndicator = new ePlayerHealthIndicator(window());
    mExperienceIndicator->setColor(eColors::sExperience);
    mExperienceIndicator->setName(eLanguage::text(7, 3));
    mExperienceIndicator->initialize(eUITextures::sExpBar2,
                                     eUITextures::sExpBar1, 6);
    centerWid->addWidget(mExperienceIndicator);

    const auto healthMana = new eWidget(window());
    healthMana->setNoPadding();

    mHealthIndicator = new ePlayerHealthIndicator(window());
    mHealthIndicator->setColor(eColors::sHealth);
    mHealthIndicator->setName(eLanguage::text(7, 0));
    mHealthIndicator->initialize(eUITextures::sLifeBar2,
                                 eUITextures::sLifeBar1, 1);
    healthMana->addWidget(mHealthIndicator);

    mManaIndicator = new ePlayerHealthIndicator(window());
    mManaIndicator->setColor(eColors::sMana);
    mManaIndicator->setName(eLanguage::text(7, 1));
    mManaIndicator->initialize(eUITextures::sLifeBar2,
                               eUITextures::sLifeBar1, 1);
    healthMana->addWidget(mManaIndicator);

    healthMana->stackHorizontally(0);
    healthMana->fitContent();
    centerWid->addWidget(healthMana);

    const auto staminaBelt = new eWidget(window());
    staminaBelt->setNoPadding();

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
    mStaminaIndicator->initialize(eUITextures::sStaminaBar2,
                                  eUITextures::sStaminaBar1, 1);
    staminaWid->addWidget(mStaminaIndicator);

    staminaWid->stackHorizontally();
    staminaWid->fitContent();

    const auto belt = new eWidget(window());
    belt->setNoPadding();
    for(int x = 0; x < 4; x++) {
        const auto slot = new eLabel(window());
        slot->setNoPadding();
        slot->setTexture(eUITextures::sEmptySlot);
        slot->fitContent();
        belt->addWidget(slot);
    }
    belt->stackHorizontally(0);
    belt->fitContent();

    staminaBelt->addWidget(staminaWid);
    staminaBelt->addWidget(belt);

    const int p = res.tinyPadding();
    staminaBelt->stackHorizontally(p);
    staminaBelt->fitContent();
    centerWid->addWidget(staminaBelt);

    centerWid->stackVertically(p);
    centerWid->fitContent();
    mExperienceIndicator->align(eAlignment::hcenter);
    centerWid->align(eAlignment::bottom | eAlignment::hcenter);
    bottomInnerWidget->addWidget(centerWid);

    mRightSkillButton = new eSkillButton(window());
    mRightSkillButton->initialize(static_cast<int>(eSkillChoice::right));
    mRightSkillButton->setPressAction([this]() {
        openSkillMenu(eAlignment::right, mRightSkillButton, mRightSkill,
                      eSkillChoice::right);
    });
    bottomInnerWidget->addWidget(mRightSkillButton);

    bottomInnerWidget->stackHorizontally();
    bottomInnerWidget->fitContent();
    mBottomWid->addWidget(bottomInnerWidget);
    mBottomWid->fitContent();
    bottomInnerWidget->align(eAlignment::center);
    addWidget(mBottomWid);
    mBottomWid->align(eAlignment::bottom | eAlignment::hcenter);
    mLeftSkillButton->align(eAlignment::bottom);
    mRightSkillButton->align(eAlignment::bottom);

    const int w = width();
    const int h = height();

    const auto& attrs = mGameWidget->attributes();
    const auto& stats = mGameWidget->stats();

    mDragWidget = new eItemDragWidget(attrs, stats, window());
    mDragWidget->resize(w, h);
    mDragWidget->initialize([this](SDL_Point pos) {
        if(mInventoryMenu) {
            pos.x -= mInventoryMenu->x();
            pos.y -= mInventoryMenu->y();
            if(pos.x < 0) {
                mGameWidget->dropItem();
            } else {
                const bool r = mInventoryMenu->dropItem(pos);
            }
        } else {
            mGameWidget->dropItem();
        }
    });
    addWidget(mDragWidget);
}

bool eGameScreen::keyPressEvent(const eKeyPressEvent& e) {
    const auto key = e.key();
    if(key == SDL_SCANCODE_ESCAPE) {
        if(mMessage) {
            mMessage->deleteLater();
            mMessage = nullptr;
        } else if(mStatsMenu) {
            hideStatsMenu();
        } else if(mInventoryMenu) {
            hideInventoryMenu();
        } else if(mSkillTreesMenu) {
            hideSkillTreesMenu();
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
    } else if(!mMessage && key == SDL_SCANCODE_R) {
        const bool run = mGameWidget->switchRunning();
        mRunButton->setChecked(run);
    } else if(!mMessage && key == SDL_SCANCODE_I) {
        if(mInventoryMenu) {
            hideInventoryMenu();
        } else {
            if(mSkillTreesMenu) {
                hideSkillTreesMenu();
            }
            showInventoryMenu();
        }
    } else if(!mMessage && key == SDL_SCANCODE_W) {
        mGameWidget->switchWeapons();
        if(mInventoryMenu) {
            mInventoryMenu->updateWeapons();
        }
    } else if(!mMessage && key == SDL_SCANCODE_A) {
        if(mStatsMenu) {
            hideStatsMenu();
        } else {
            showStatsMenu();
        }
    } else if(!mMessage && key == SDL_SCANCODE_T) {
        if(mSkillTreesMenu) {
            hideSkillTreesMenu();
        } else {
            if(mInventoryMenu) {
                hideInventoryMenu();
            }
            showSkillTreesMenu();
        }
    } else if(key == SDL_SCANCODE_RETURN) {
        if(mMessage) {
            const auto& text = mMessage->text();
            if(!text.empty()) {
                mGameWidget->sendMessage(text);
            }
            hideMessageBox();
        } else {
            showMessageBox();
        }
    } else if(key == SDL_SCANCODE_F1) {
        hotkeyPressed(1);
    } else if(key == SDL_SCANCODE_F2) {
        hotkeyPressed(2);
    } else if(key == SDL_SCANCODE_F3) {
        hotkeyPressed(3);
    } else if(key == SDL_SCANCODE_F4) {
        hotkeyPressed(4);
    } else if(key == SDL_SCANCODE_F5) {
        hotkeyPressed(5);
    } else if(key == SDL_SCANCODE_F6) {
        hotkeyPressed(6);
    } else if(key == SDL_SCANCODE_F7) {
        hotkeyPressed(7);
    } else if(key == SDL_SCANCODE_F8) {
        hotkeyPressed(8);
    } else {
        return false;
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
    mExperienceIndicator->setValue(attrs.fExp);
}

void eGameScreen::hotkeyPressed(const int fkey) {
    const auto itL = eSkillButton::sLeftMap.find(fkey);
    if(itL != eSkillButton::sLeftMap.end()) {
        const int skillId = itL->second;
        mGameWidget->setLeftSkill(skillId);
        mLeftSkillButton->setSkillId(skillId);
        return;
    }
    const auto itR = eSkillButton::sRightMap.find(fkey);
    if(itR != eSkillButton::sRightMap.end()) {
        const int skillId = itR->second;
        mGameWidget->setRightSkill(skillId);
        mRightSkillButton->setSkillId(skillId);
        return;
    }
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

    const auto& res = resolution();
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
        mGameWidget->save();
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
    auto& stats = mGameWidget->stats();
    addWidget(mInventoryMenu);
    mInventoryMenu->align(eAlignment::right | eAlignment::top);
    mInventoryMenu->initialize(eq, stats);
    eItemDragWidget::sUpdateDragItem(eq);

    updateCharPos();
    mDragWidget->bringToFront();
}

void eGameScreen::hideInventoryMenu() {
    if(!mInventoryMenu) return;
    mInventoryMenu->deleteLater();
    mInventoryMenu = nullptr;
    updateCharPos();
    mDragWidget->setHoverItem(eItem());
}

void eGameScreen::showStatsMenu() {
    if(mStatsMenu) return;
    mStatsMenu = new eStatsWidget(window());
    const int w = width();
    const int h = height();
    mStatsMenu->resize(w/2, h - mBottomWid->height());
    const auto& cname = mGameWidget->cname();
    auto& stats = mGameWidget->stats();
    const auto& eq = mGameWidget->equipment();
    auto& attrs = mGameWidget->attributes();
    mStatsMenu->initialize(cname, stats, eq, attrs);
    addWidget(mStatsMenu);
    mStatsMenu->align(eAlignment::left | eAlignment::top);
    updateCharPos();
    mDragWidget->bringToFront();
}

void eGameScreen::hideStatsMenu() {
    if(!mStatsMenu) return;
    mStatsMenu->deleteLater();
    mStatsMenu = nullptr;
    updateCharPos();
}

void eGameScreen::showSkillTreesMenu() {
    if(mSkillTreesMenu) return;
    mSkillTreesMenu = new eSkillTreesWidget(window());
    const int w = width();
    const int h = height();
    mSkillTreesMenu->resize(w/2, h - mBottomWid->height());
    const auto& cname = mGameWidget->cname();
    auto& stats = mGameWidget->stats();
    const auto& eq = mGameWidget->equipment();
    const auto& attrs = mGameWidget->attributes();
    mSkillTreesMenu->initialize(stats, attrs, eq);
    addWidget(mSkillTreesMenu);
    mSkillTreesMenu->align(eAlignment::right | eAlignment::top);
    updateCharPos();
    mDragWidget->bringToFront();
}

void eGameScreen::hideSkillTreesMenu() {
    if(!mSkillTreesMenu) return;
    mSkillTreesMenu->deleteLater();
    mSkillTreesMenu = nullptr;
    updateCharPos();
    mDragWidget->setHoverSkill(-1, false);
}

void eGameScreen::showMessageBox() {
    if(mMessage) return;
    const auto window = eWidget::window();
    window->startTextInput();

    mMessage = new eLineEdit(window);
    const int p = mMessage->padding();
    const auto& res = resolution();
    const int fontSize = res.smallFontSize();
    const auto font = eFonts::textFont(fontSize);
    mMessage->setFont(font);
    mMessage->allow(' ');
    mMessage->allow(',');
    mMessage->allow('.');
    mMessage->allow('?');
    mMessage->allow('!');
    mMessage->allow(';');
    mMessage->allow(':');
    mMessage->allow('\'');
    mMessage->allow('"');
    mMessage->allow('\\');
    mMessage->allow('/');
    mMessage->allow('@');
    mMessage->allow('#');
    mMessage->allow('$');
    mMessage->allow('%');
    mMessage->allow('&');
    mMessage->allow('*');
    mMessage->allow('+');
    mMessage->allow('(');
    mMessage->allow(')');
    mMessage->allow('[');
    mMessage->allow(']');
    mMessage->setMaxLength(1000);
    const int w = width()/2;
    const int h = height()/10;
    mMessage->setWrapWidth(w - 2*p);
    mMessage->resize(w, h);
    mMessage->setTextAlignment(eAlignment::left | eAlignment::top);
    mMessage->grabKeyboard();
    addWidget(mMessage);
    mMessage->move(w/2, mBottomWid->y() - mMessage->height() - p);
}

void eGameScreen::hideMessageBox() {
    if(!mMessage) return;
    const auto window = eWidget::window();
    window->stopTextInput();
    mMessage->deleteLater();
    mMessage = nullptr;
}

void eGameScreen::openSkillMenu(const eAlignment align,
                                eSkillButton* const targetButton,
                                int& targetSkillVar,
                                const eSkillChoice schoice) {
    if(mSkillMenu) {
        mSkillMenu->deleteLater();
        mSkillMenu = nullptr;
        return;
    }

    const auto w = new eSkillSelectWidget(window());

    std::vector<int> skillIds;
    const auto& stats = mGameWidget->stats();
    const auto& skillLevels = stats.fBaseSkillLevels;
    for(const auto& s : skillLevels) {
        skillIds.push_back(s.first);
    }

    const auto action = [this, targetButton, &targetSkillVar](const int skillId) {
        targetButton->setSkillId(skillId);
        targetSkillVar = skillId;
        mGameWidget->setLeftSkill(mLeftSkill);
        mGameWidget->setRightSkill(mRightSkill);
        mSkillMenu = nullptr;
    };
    w->initialize(skillIds, align, action, schoice);

    addWidget(w);

    const auto& res = resolution();
    const float mult = res.multiplier();
    const int margin = 100*mult;
    w->move(align == eAlignment::left ? margin : width() - w->width() - margin,
            height() - w->height() - margin);

    mSkillMenu = w;
    mDragWidget->bringToFront();
}

void eGameScreen::updateCharPos() {
    auto& input = mGameWidget->input();
    const bool left = mStatsMenu;
    const bool right = mInventoryMenu || mSkillTreesMenu;
    if(left && right) {
        input.setCharacterHorizontalPos(0.5f);
    } else if(left) {
        input.setCharacterHorizontalPos(0.75f);
    } else if(right) {
        input.setCharacterHorizontalPos(0.25f);
    } else {
        input.setCharacterHorizontalPos(0.5f);
    }
}

#include "egamescreen.h"

#include "../etext.h"
#include "../emainwindow.h"
#include "../widgets/elineedit.h"

#include "../widgets/gameScreen/eescmenu.h"
#include "../widgets/gameScreen/egamewidget.h"
#include "../widgets/gameScreen/einventorywidget.h"
#include "../widgets/gameScreen/ehoverwidget.h"
#include "../widgets/gameScreen/eminimap.h"
#include "../widgets/gameScreen/estatuswidget.h"
#include "../widgets/gameScreen/eskillbutton.h"
#include "../widgets/gameScreen/eskillselectwidget.h"
#include "../widgets/gameScreen/eskilltreeswidget.h"
#include "../widgets/gameScreen/estatswidget.h"
#include "../widgets/gameScreen/eunitindicator.h"
#include "../widgets/gameScreen/epartywidget.h"
#include "../widgets/gameScreen/ewaypointwidget.h"
#include "../widgets/gameScreen/ebottomwidget.h"
#include "../widgets/gameScreen/estashwidget.h"
#include "../widgets/gameScreen/esellerwidget.h"

#include <eSlayerHelpers/epotiontype.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/egamedir.h>
#include <eSlayerHelpers/eskills.h>
#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eitemsdata.h>

eGameScreen* eGameScreen::sInstance = nullptr;

eGameScreen::eGameScreen(eMainWindow* const window) :
    eScreenBase(window) {
    sInstance = this;
}

void eGameScreen::setExitAction(const eAction& a) {
    mExitAction = a;
}

void eGameScreen::initialize(const uint32_t clientId,
                             const std::shared_ptr<eServer>& server,
                             const std::shared_ptr<eMap>& map,
                             const eCharacter& c,
                             const eTeamId teamId,
                             const eMoveToMapAction& move) {
    eSkillButton::sLeftMap = c.leftHotkeys();
    eSkillButton::sRightMap = c.rightHotkeys();

    mGameWidget = new eGameWidget(window());
    mGameWidget->resize(width(), height());
    addWidget(mGameWidget);

    mGameWidget->initialize(clientId, server, map, c, teamId, move);

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
    const auto& res = resolution();
    const float m = res.multiplier();
    mUnitIndicator->resize(200*m, 40*m);
    addWidget(mUnitIndicator);
    mUnitIndicator->align(eAlignment::hcenter | eAlignment::top);
    mUnitIndicator->setY(20*m);
    mUnitIndicator->initialize();

    mGameWidget->setUnitIndicator(mUnitIndicator);

    const auto& stats = mGameWidget->stats();
    auto& eq = mGameWidget->equipment();

    const auto leftSkillA = [this]() {
        const auto button = mBottomWidget->leftSkillButton();
        openSkillMenu(eAlignment::left, button,
                      mLeftSkill, eSkillChoice::left);
    };
    const auto rightSkillA = [this]() {
        const auto button = mBottomWidget->rightSkillButton();
        openSkillMenu(eAlignment::right, button,
                      mRightSkill, eSkillChoice::right);
    };

    const auto runA = [this](const bool check) {
        auto& action = mGameWidget->mainAction();
        action.setRunning(check);
    };
    const auto portalA = [this](const bool) {
        mGameWidget->spawnPortal();
    };

    const auto invA = [this](const bool) {
        if(mInventoryMenu) {
            hideInventoryConnectedMenu();
        } else {
            hideRightMenu();
            showInventoryMenu();
        }
    };

    const auto attrsA = [this](const bool) {
        if(mStatsMenu) {
            hideStatsMenu();
        } else {
            hideLeftMenu();
            showStatsMenu();
        }
    };

    const auto skillA = [this](const bool) {
        if(mSkillTreesMenu) {
            hideSkillTreesMenu();
        } else {
            hideRightMenu();
            showSkillTreesMenu();
        }
    };

    const auto partyA = [this](const bool) {
        if(mPartyMenu) {
            hidePartyMenu();
        } else {
            hideLeftMenu();
            showPartyMenu();
        }
    };

    mBottomWidget = new eBottomWidget(
        stats, eq, window());
    mBottomWidget->initialize(
        leftSkillA, c.leftSkill(),
        rightSkillA, c.rightSkill(),
        runA, portalA, invA, attrsA,
        skillA, partyA);
    addWidget(mBottomWidget);
    mBottomWidget->align(eAlignment::bottom | eAlignment::hcenter);

    const int w = width();
    const int h = height();
    mMenusWidget = new eWidget(window());
    mMenusWidget->resize(w, h);
    addWidget(mMenusWidget);

    mMiniMap = new eMiniMap(window());
    mMiniMap->resize(width(), height());
    eGameSettings settings;
    settings.fType = server->name();
    settings.fIP = server->ip();
    settings.fPassword = server->password();
    mMiniMap->initialize(settings);
    mMiniMap->setMap(map);
    addWidget(mMiniMap);

    const auto& attrs = mGameWidget->attributes();

    mDragWidget = new eHoverWidget(attrs, stats, window());
    mDragWidget->resize(w, h);
    mDragWidget->initialize([this]() {
        const bool r = mBottomWidget->dropItem();
        if(r) return;
        if(mInventoryMenu) {
            const bool r = mInventoryMenu->dropItem();
            if(r) return;
            const bool h = mInventoryMenu->hovered();
            if(h) return;
        }
        if(mStashMenu) {
            const bool r = mStashMenu->dropItem();
            if(r) return;
            const bool h = mStashMenu->hovered();
            if(h) return;
        }
        if(mSellerMenu) {
            const bool r = mSellerMenu->dropItem();
            if(r) return;
            const bool h = mSellerMenu->hovered();
            if(h) return;
        }
        mGameWidget->dropItem();
    });
    addWidget(mDragWidget);

    mLeftSkill = c.leftSkill();
    mRightSkill = c.rightSkill();
    mOtherLeftSkill = c.otherLeftSkill();
    mOtherRightSkill = c.otherRightSkill();
    mGameWidget->setOtherLeftSkill(mOtherLeftSkill);
    mGameWidget->setOtherRightSkill(mOtherRightSkill);
}

void eGameScreen::sOpenWaypointMenu(
    const uint8_t actId,
    const eAreaIds& area) {
    sInstance->showWaypointMenu(actId, area);
}

void eGameScreen::sOpenStash() {
    sInstance->showStashMenu();
}

void eGameScreen::sOpenSellerMenu(
    const eSeller& s) {
    sInstance->showSellerMenu(s);
}

void eGameScreen::sCloseObjectMenu() {
    sInstance->hidePositionedMenu();
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
            hideInventoryConnectedMenu();
        } else if(mSkillTreesMenu) {
            hideSkillTreesMenu();
        } else if(mPartyMenu) {
            hidePartyMenu();
        } else if(mWaypointMenu) {
            hideWaypointMenu();
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
        mBottomWidget->setRunning(run);
    } else if(!mMessage && key == SDL_SCANCODE_I) {
        if(mInventoryMenu) {
            hideInventoryConnectedMenu();
        } else {
            hideRightMenu();
            showInventoryMenu();
        }
    } else if(!mMessage && key == SDL_SCANCODE_W) {
        const int leftTmp = mLeftSkill;
        setLeftSkill(mOtherLeftSkill);
        mOtherLeftSkill = leftTmp;
        mGameWidget->setOtherLeftSkill(leftTmp);

        const int rightTmp = mRightSkill;
        setRightSkill(mOtherRightSkill);
        mOtherRightSkill = rightTmp;
        mGameWidget->setOtherRightSkill(rightTmp);

        mGameWidget->switchWeapons();
        if(mInventoryMenu) {
            mInventoryMenu->updateWeapons();
        }
    } else if(!mMessage && key == SDL_SCANCODE_A) {
        if(mStatsMenu) {
            hideStatsMenu();
        } else {
            hideLeftMenu();
            showStatsMenu();
        }
    } else if(!mMessage && key == SDL_SCANCODE_P) {
        if(mPartyMenu) {
            hidePartyMenu();
        } else {
            hideLeftMenu();
            showPartyMenu();
        }
    } else if(!mMessage && key == SDL_SCANCODE_T) {
        if(mSkillTreesMenu) {
            hideSkillTreesMenu();
        } else {
            hideRightMenu();
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
    } else if(key == SDL_SCANCODE_GRAVE) {
        mBottomWidget->switchBeltVisible();
    } else if(key == SDL_SCANCODE_TAB) {
        mMiniMap->switchShowMap();
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
    } else if(key == SDL_SCANCODE_1) {
        consumePotion(0);
    } else if(key == SDL_SCANCODE_2) {
        consumePotion(1);
    } else if(key == SDL_SCANCODE_3) {
        consumePotion(2);
    } else if(key == SDL_SCANCODE_4) {
        consumePotion(3);
    } else if(key == SDL_SCANCODE_5) {
        consumePotion(4);
    } else if(key == SDL_SCANCODE_6) {
        consumePotion(5);
    } else {
        return false;
    }
    return true;
}

void eGameScreen::paintEvent(ePainter&) {
    const auto& stats = mGameWidget->stats();
    const auto& attrs = mGameWidget->attributes();

    const auto& action = mGameWidget->mainAction();

    const auto& mainChar = mGameWidget->mainChar();
    const bool staminaPotion = mainChar.staminaPotion();
    const bool poisoned = mainChar.poisoned();

    mBottomWidget->setIndicators(
        stats.fHealthF, stats.fMaxHealth,
        stats.fManaF, stats.fMaxMana,
        stats.fStaminaF, stats.fMaxStamina,
        staminaPotion, poisoned,
        attrs.fExp, attrs.nextLevelExp());

    const auto& pos = action.pos();
    mMiniMap->setPos(pos);
}

void eGameScreen::setLeftSkill(const int skillId) {
    mLeftSkill = skillId;
    mGameWidget->setLeftSkill(skillId);
    mBottomWidget->setLeftSkill(skillId);
}

void eGameScreen::setRightSkill(const int skillId) {
    mRightSkill = skillId;
    mGameWidget->setRightSkill(skillId);
    mBottomWidget->setRightSkill(skillId);
}

void eGameScreen::hotkeyPressed(const int fkey) {
    const auto itL = eSkillButton::sLeftMap.find(fkey);
    if(itL != eSkillButton::sLeftMap.end()) {
        const int skillId = itL->second;
        setLeftSkill(skillId);
        return;
    }
    const auto itR = eSkillButton::sRightMap.find(fkey);
    if(itR != eSkillButton::sRightMap.end()) {
        const int skillId = itR->second;
        setRightSkill(skillId);
        return;
    }
}

void eGameScreen::consumePotion(const int x) {
    return mGameWidget->consumePotion(x);
}

void eGameScreen::hidePositionedMenu() {
    hideWaypointMenu();
    if(mStashMenu || mSellerMenu) {
        hideInventoryConnectedMenu();
    }
    updateCharPos();
}

void eGameScreen::hideLeftMenu() {
    if(mPartyMenu) {
        hidePartyMenu();
    }
    if(mStatsMenu) {
        hideStatsMenu();
    }
    if(mWaypointMenu) {
        hideWaypointMenu();
    }
    if(mStashMenu) {
        hideInventoryConnectedMenu();
    }
    if(mSellerMenu) {
        hideInventoryConnectedMenu();
    }
}

void eGameScreen::hideRightMenu() {
    if(mSkillTreesMenu) {
        hideSkillTreesMenu();
    }
    if(mInventoryMenu) {
        hideInventoryConnectedMenu();
    }
}

void eGameScreen::showDeadMenu() {
    mDeadMenu = new eWidget(window());

    const auto line1 = new eLabel(window());
    line1->setExtraHugeFontSize();
    line1->setFontColor(eFontColor::redBlack);
    line1->setText(eText::text(5, 3));
    line1->fitContent();
    mDeadMenu->addWidget(line1);

    const auto line2 = new eLabel(window());
    line2->setExtraHugeFontSize();
    line2->setFontColor(eFontColor::redBlack);
    line2->setText(eText::text(5, 4));
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

void eGameScreen::showInventoryMenu(
    const eHoverItemType htype) {
    if(mInventoryMenu) return;
    mInventoryMenu = new eInventoryWidget(window());
    const int w = width();
    const int h = height();
    mInventoryMenu->resize(w/2, h - mBottomWidget->height());
    auto& eq = mGameWidget->equipment();
    auto& stats = mGameWidget->stats();
    mMenusWidget->addWidget(mInventoryMenu);
    mInventoryMenu->initialize(eq, stats, htype);
    mInventoryMenu->align(eAlignment::right | eAlignment::top);
    mInventoryMenu->updateWeapons();
    eHoverWidget::sUpdateDragItem(eq);

    updateCharPos();
}

void eGameScreen::hideInventoryMenu() {
    if(!mInventoryMenu) return;
    mInventoryMenu->deleteLater();
    mInventoryMenu = nullptr;
    updateCharPos();
    eHoverItem hitem;
    mDragWidget->setHoverItem(hitem);
}

void eGameScreen::showPartyMenu() {
    if(mPartyMenu) return;
    mPartyMenu = new ePartyWidget(window());
    const int w = width();
    const int h = height();
    mPartyMenu->resize(w/2, h - mBottomWidget->height());
    const auto& cname = mGameWidget->cname();
    mPartyMenu->initialize(cname);
    mMenusWidget->addWidget(mPartyMenu);
    mPartyMenu->align(eAlignment::left | eAlignment::top);
    updateCharPos();
}

void eGameScreen::hidePartyMenu() {
    if(!mPartyMenu) return;
    mPartyMenu->deleteLater();
    mPartyMenu = nullptr;
    updateCharPos();
}

void eGameScreen::showStatsMenu() {
    if(mStatsMenu) return;
    mStatsMenu = new eStatsWidget(window());
    const int w = width();
    const int h = height();
    mStatsMenu->resize(w/2, h - mBottomWidget->height());
    const auto& cname = mGameWidget->cname();
    auto& stats = mGameWidget->stats();
    const auto& eq = mGameWidget->equipment();
    auto& attrs = mGameWidget->attributes();
    mStatsMenu->initialize(cname, stats, eq, attrs);
    mMenusWidget->addWidget(mStatsMenu);
    mStatsMenu->align(eAlignment::left | eAlignment::top);
    updateCharPos();
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
    mSkillTreesMenu->resize(w/2, h - mBottomWidget->height());
    const auto& cname = mGameWidget->cname();
    auto& stats = mGameWidget->stats();
    const auto& eq = mGameWidget->equipment();
    const auto& attrs = mGameWidget->attributes();
    mSkillTreesMenu->initialize(stats, attrs, eq);
    mMenusWidget->addWidget(mSkillTreesMenu);
    mSkillTreesMenu->align(eAlignment::right | eAlignment::top);
    updateCharPos();
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
    mMessage->move(w/2, mBottomWidget->y() - mMessage->height() - p);
}

void eGameScreen::hideMessageBox() {
    if(!mMessage) return;
    const auto window = eWidget::window();
    window->stopTextInput();
    mMessage->deleteLater();
    mMessage = nullptr;
}

void eGameScreen::showWaypointMenu(
    const uint8_t cActId,
    const eAreaIds& cArea) {
    if(mWaypointMenu) return;
    mWaypointMenu = new eWaypointWidget(window());
    const int w = width();
    const int h = height();
    mWaypointMenu->resize(w/2, h - mBottomWidget->height());
    const auto action = [this](const eAreaIds& area) {
        mGameWidget->waypointTeleport(area);
    };
    mWaypointMenu->initialize(
        cActId, cArea, action);
    mMenusWidget->addWidget(mWaypointMenu);
    mWaypointMenu->align(eAlignment::left | eAlignment::top);
    updateCharPos();
}

void eGameScreen::hideWaypointMenu() {
    if(!mWaypointMenu) return;
    mWaypointMenu->deleteLater();
    mWaypointMenu = nullptr;
    updateCharPos();
}

void eGameScreen::showSellerMenu(
    const eSeller& seller) {
    if(mSellerMenu) return;
    mSellerMenu = new eSellerWidget(window());
    const int w = width();
    const int h = height();
    mSellerMenu->resize(w/2, h - mBottomWidget->height());
    const auto clientId = mGameWidget->clientId();
    auto& eq = mGameWidget->equipment();
    const auto& stats = mGameWidget->stats();
    mSellerMenu->initialize(clientId, seller, eq, stats);
    mMenusWidget->addWidget(mSellerMenu);
    mSellerMenu->align(eAlignment::left | eAlignment::top);

    showInventoryMenu(eHoverItemType::sell);

    updateCharPos();
}

void eGameScreen::hideSellerMenu() {
    if(!mSellerMenu) return;
    mSellerMenu->deleteLater();
    mSellerMenu = nullptr;
    updateCharPos();
}

void eGameScreen::showStashMenu() {
    if(mStashMenu) return;
    mStashMenu = new eStashWidget(window());
    const int w = width();
    const int h = height();
    mStashMenu->resize(w/2, h - mBottomWidget->height());
    auto& eq = mGameWidget->equipment();
    auto& stats = mGameWidget->stats();
    mMenusWidget->addWidget(mStashMenu);
    mStashMenu->initialize(eq, stats);
    mStashMenu->align(eAlignment::left | eAlignment::top);
    eHoverWidget::sUpdateDragItem(eq);

    showInventoryMenu();

    updateCharPos();
}

void eGameScreen::hideStashMenu() {
    if(!mStashMenu) return;
    mStashMenu->deleteLater();
    mStashMenu = nullptr;
    updateCharPos();
}

void eGameScreen::hideInventoryConnectedMenu() {
    hideInventoryMenu();
    hideSellerMenu();
    hideStashMenu();
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

    mMenusWidget->addWidget(w);

    const auto& res = resolution();
    const float mult = res.multiplier();
    const int margin = 100*mult;
    w->move(align == eAlignment::left ? margin : width() - w->width() - margin,
            height() - w->height() - margin);

    mSkillMenu = w;
}

void eGameScreen::updateCharPos() {
    auto& input = mGameWidget->input();
    const bool left = mStatsMenu || mPartyMenu ||
                      mWaypointMenu || mStashMenu ||
                      mSellerMenu;
    const bool right = mInventoryMenu || mSkillTreesMenu;
    float hpos = 0.5f;
    if(left && right) {
        hpos = 0.5f;
    } else if(left) {
        hpos = 0.75f;
    } else if(right) {
        hpos = 0.25f;
    } else {
        hpos = 0.5f;
    }
    input.setCharacterHorizontalPos(hpos);
    mMiniMap->setCharacterHorizontalPos(hpos);
}

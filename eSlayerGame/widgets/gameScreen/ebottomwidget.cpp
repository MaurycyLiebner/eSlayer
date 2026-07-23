#include "ebottomwidget.h"

#include "eskillbutton.h"
#include "eplayerhealthindicator.h"
#include "einventorybagpackwidget.h"
#include "../etexturecheckbutton.h"
#include "../../etext.h"
#include "../ecolors.h"
#include "../../textures/euitextures.h"

#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eattributes.h>
#include <eSlayerHelpers/eequipment.h>

eBottomWidget::eBottomWidget(
    const uint32_t clientId,
    const eStats& stats,
    const eAttributes& attrs,
    eEquipment& eq,
    eMainWindow* const window) :
    eWidget(window),
    mClientId(clientId),
    mStats(stats),
    mAttrs(attrs),
    mEq(eq) {}

void eBottomWidget::initialize(const eAction& leftSkillA,
                               const int leftSkillId,
                               const eAction& rightSkillA,
                               const int rightSkillId,
                               const eCheckAction& runA,
                               const eCheckAction& portalA,
                               const eCheckAction& invA,
                               const eCheckAction& attrsA,
                               const eCheckAction& skillA,
                               const eCheckAction& partyA,
                               const eCheckAction& questsA,
                               const eCheckAction& automapA,
                               const eCheckAction& messagesA,
                               const eCheckAction& gameMenuA) {
    setNoPadding();

    mLeftSkillButton = new eSkillButton(window());
    mLeftSkillButton->initialize(static_cast<int>(eSkillChoice::left));
    mLeftSkillButton->setPressAction(leftSkillA);
    addWidget(mLeftSkillButton);
    mLeftSkillButton->setSkillId(leftSkillId);
    mLeftSkillId = leftSkillId;

    const auto& res = resolution();
    const int p = res.tinyPadding();

    const auto centerWid = new eWidget(window());
    centerWid->setNoPadding();

    const auto spaceW = new eWidget(window());
    spaceW->setHugePadding();
    spaceW->fitContent();
    centerWid->addWidget(spaceW);

    const auto buttonW = new eWidget(window());
    buttonW->setNoPadding();

    const auto inventoryButton = new eTextureCheckButton(window());
    inventoryButton->setTooltip(eText::text(18, 1));
    inventoryButton->setCheckAction(invA);
    inventoryButton->initialize(eUITextures::sInventoryIcon,
                                eUITextures::sInventoryIcon);
    buttonW->addWidget(inventoryButton);

    const auto attributesButton = new eTextureCheckButton(window());
    attributesButton->setTooltip(eText::text(18, 2));
    attributesButton->setCheckAction(attrsA);
    attributesButton->initialize(eUITextures::sAttributesIcon,
                                 eUITextures::sAttributesIcon);
    buttonW->addWidget(attributesButton);

    const auto skillsButton = new eTextureCheckButton(window());
    skillsButton->setTooltip(eText::text(18, 3));
    skillsButton->setCheckAction(skillA);
    skillsButton->initialize(eUITextures::sSkillsIcon,
                             eUITextures::sSkillsIcon);
    buttonW->addWidget(skillsButton);

    const auto partyButton = new eTextureCheckButton(window());
    partyButton->setTooltip(eText::text(18, 4));
    partyButton->setCheckAction(partyA);
    partyButton->initialize(eUITextures::sPartyIcon,
                            eUITextures::sPartyIcon);
    buttonW->addWidget(partyButton);

    const auto questsButton = new eTextureCheckButton(window());
    questsButton->setTooltip(eText::text(18, 13));
    questsButton->setCheckAction(questsA);
    questsButton->initialize(eUITextures::sQuestsIcon,
                             eUITextures::sQuestsIcon);
    buttonW->addWidget(questsButton);

    const auto automapButton = new eTextureCheckButton(window());
    automapButton->setTooltip(eText::text(18, 15));
    automapButton->setCheckAction(automapA);
    automapButton->initialize(eUITextures::sAutomapIcon,
                              eUITextures::sAutomapIcon);
    buttonW->addWidget(automapButton);

    const auto messagesButton = new eTextureCheckButton(window());
    messagesButton->setTooltip(eText::text(18, 14));
    messagesButton->setCheckAction(messagesA);
    messagesButton->initialize(eUITextures::sMessagesIcon,
                               eUITextures::sMessagesIcon);
    buttonW->addWidget(messagesButton);

    const auto gameMenuButton = new eTextureCheckButton(window());
    gameMenuButton->setTooltip(eText::text(18, 16));
    gameMenuButton->setCheckAction(gameMenuA);
    gameMenuButton->initialize(eUITextures::sGameMenuIcon,
                               eUITextures::sGameMenuIcon);
    buttonW->addWidget(gameMenuButton);

    buttonW->stackHorizontally();
    buttonW->fitContent();
    centerWid->addWidget(buttonW);

    mExperienceIndicator = new ePlayerHealthIndicator(window());
    mExperienceIndicator->setColor(eColors::sExperience);
    mExperienceIndicator->setName(eText::text(7, 3));
    mExperienceIndicator->initialize(eUITextures::sExpBar2,
                                     eUITextures::sExpBar1, 6);
    centerWid->addWidget(mExperienceIndicator);

    const auto healthMana = new eWidget(window());
    healthMana->setNoPadding();

    mHealthIndicator = new ePlayerHealthIndicator(window());
    mHealthIndicator->setColor(eColors::sHealth);
    mHealthIndicator->setName(eText::text(7, 0));
    mHealthIndicator->initialize(eUITextures::sLifeBar2,
                                 eUITextures::sLifeBar1, 1);
    healthMana->addWidget(mHealthIndicator);

    mManaIndicator = new ePlayerHealthIndicator(window());
    mManaIndicator->setColor(eColors::sMana);
    mManaIndicator->setName(eText::text(7, 1));
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
    mRunButton->setTooltip(eText::text(18, 5));
    mRunButton->setCheckAction(runA);
    mRunButton->initialize(eUITextures::sRunIcon,
                           eUITextures::sWalkIcon);
    staminaWid->addWidget(mRunButton);

    mStaminaIndicator = new ePlayerHealthIndicator(window());
    mStaminaIndicator->setColor(eColors::sStamina);
    mStaminaIndicator->setName(eText::text(7, 2));
    mStaminaIndicator->initialize(eUITextures::sStaminaBar2,
                                  eUITextures::sStaminaBar1, 1);
    staminaWid->addWidget(mStaminaIndicator);

    staminaWid->stackHorizontally();
    staminaWid->fitContent();

    staminaBelt->addWidget(staminaWid);

    mNewStats = new eTextureCheckButton(window());
    mNewStats->initialize(eUITextures::sNewFalseSmallIcon,
                          eUITextures::sNewFalseSmallIcon);
    mNewStats->setTooltip(eText::text(18, 17));
    mNewStats->setCheckAction([attrsA](const bool c) {
        if(attrsA) attrsA(c);
    });
    staminaBelt->addWidget(mNewStats);

    mBelt = new eInventoryBagpackWidget(window());
    mBelt->initialize(mClientId,
                      eEquipment::fBeltHPotionSlots,
                      1, mEq.fBeltPotions,
                      mEq, eBagpackType::belt,
                      eHoverItemType::regular);

    staminaBelt->addWidget(mBelt);

    mNewSkill = new eTextureCheckButton(window());
    mNewSkill->initialize(eUITextures::sNewFalseSmallIcon,
                          eUITextures::sNewFalseSmallIcon);
    mNewSkill->setTooltip(eText::text(18, 18));
    mNewSkill->setCheckAction([skillA](const bool c) {
        if(skillA) skillA(c);
    });
    staminaBelt->addWidget(mNewSkill);

    const auto portalButton = new eTextureCheckButton(window());
    portalButton->setTooltip(eText::text(18, 0));
    portalButton->setCheckAction(portalA);
    portalButton->initialize(eUITextures::sPortalIcon,
                             eUITextures::sPortalIcon);
    staminaBelt->addWidget(portalButton);

    staminaBelt->stackHorizontally(p);
    staminaBelt->fitContent();
    centerWid->addWidget(staminaBelt);

    centerWid->stackVertically(p);
    centerWid->fitContent();
    mExperienceIndicator->align(eAlignment::hcenter);
    buttonW->align(eAlignment::hcenter);
    centerWid->align(eAlignment::bottom | eAlignment::hcenter);
    addWidget(centerWid);

    mRightSkillButton = new eSkillButton(window());
    mRightSkillButton->initialize(static_cast<int>(eSkillChoice::right));
    mRightSkillButton->setPressAction(rightSkillA);
    addWidget(mRightSkillButton);
    mRightSkillButton->setSkillId(rightSkillId);
    mRightSkillId = rightSkillId;

    stackHorizontally();
    fitContent();
    const int hp = res.hugePadding();
    mLeftSkillButton->align(eAlignment::bottom);
    mLeftSkillButton->setY(mLeftSkillButton->y() - hp);
    mRightSkillButton->align(eAlignment::bottom);
    mRightSkillButton->setY(mRightSkillButton->y() - hp);
}

void eBottomWidget::setLeftSkill(const int skillId) {
    mLeftSkillId = skillId;
    mLeftSkillButton->setSkillId(skillId);
}

void eBottomWidget::setRightSkill(const int skillId) {
    mRightSkillId = skillId;
    mRightSkillButton->setSkillId(skillId);
}

void eBottomWidget::setIndicators(
    const int health,
    const int maxHealth,
    const int mana,
    const int maxMana,
    const int stamina,
    const int maxStamina,
    const bool staminaPotion,
    const bool poisoned,
    const int exp,
    const int maxExp) {
    const auto healthColor = poisoned ?
        eColors::sHealthPoisoned :
        eColors::sHealth;
    mHealthIndicator->setRange(0, maxHealth);
    mHealthIndicator->setValue(health);
    mHealthIndicator->setColor(healthColor);

    mManaIndicator->setRange(0, maxMana);
    mManaIndicator->setValue(mana);

    mStaminaIndicator->setRange(0, maxStamina);
    mStaminaIndicator->setValue(stamina);
    const auto staminaColor = staminaPotion ?
        eColors::sStaminaPotion :
        eColors::sStamina;
    mStaminaIndicator->setColor(staminaColor);

    mExperienceIndicator->setRange(0, maxExp);
    mExperienceIndicator->setValue(exp);
}

void eBottomWidget::setRunning(const bool r) {
    mRunButton->setChecked(r);
}

bool eBottomWidget::dropItem() const {
    const bool r = mBelt->dropItem();
    if(r) return true;
    const bool rr = mBeltExt && mBeltExt->dropItem();
    return rr;
}

void eBottomWidget::switchBeltVisible() {
    if(mBeltExt) {
        if(mBeltExtTmp) {
            mBeltExtTmp = false;
        } else {
            hideBeltExt();
            mBeltExtTmp = true;
        }
    } else {
        showBeltExt();
        mBeltExtTmp = false;
    }
}

void eBottomWidget::paintEvent(ePainter& p) {
    if(!mBeltExt && mBelt->hovered()) {
        showBeltExt();
        mBeltExtTmp = true;
    } else if(mBeltExt && mBeltExtTmp &&
              !mBelt->hovered() && !mBeltExt->hovered()) {
        hideBeltExt();
    }

    const bool statsEnabled = mAttrs.fStatPoints > 0;
    if(mNewStatsEnabled != statsEnabled) {
        mNewStatsEnabled = statsEnabled;
        if(statsEnabled) {
            mNewStats->initialize(eUITextures::sNewTrueSmallIcon,
                                  eUITextures::sNewTrueSmallIcon);
        } else {
            mNewStats->initialize(eUITextures::sNewFalseSmallIcon,
                                  eUITextures::sNewFalseSmallIcon);
        }
    }
    const auto& skills = mStats.fBaseSkillLevels;
    const bool skillEnabled = skills.fRemainingPoints > 0;
    if(mNewSkillEnabled != skillEnabled) {
        mNewSkillEnabled = skillEnabled;
        if(skillEnabled) {
            mNewSkill->initialize(eUITextures::sNewTrueSmallIcon,
                                  eUITextures::sNewTrueSmallIcon);
        } else {
            mNewSkill->initialize(eUITextures::sNewFalseSmallIcon,
                                  eUITextures::sNewFalseSmallIcon);
        }
    }

    const auto& cs = mStats.fCooldowns;
    {
        const float maxCooldown = mStats.cooldown(static_cast<int>(eSkillChoice::left));
        mLeftSkillButton->setCooldownMax(25.f*maxCooldown);

        const auto it = cs.find(mLeftSkillId);
        float cooldown = 0.f;
        if(it != cs.end()) {
            cooldown = it->second;
        }
        mLeftSkillButton->setCooldown(cooldown);

        mLeftSkillButton->setCanUse(mStats.canUseSkill(eSkillChoice::left));
    }

    {
        const float maxCooldown = mStats.cooldown(static_cast<int>(eSkillChoice::right));
        mRightSkillButton->setCooldownMax(25.f*maxCooldown);

        const auto it = cs.find(mRightSkillId);
        float cooldown = 0.f;
        if(it != cs.end()) {
            cooldown = it->second;
        }
        mRightSkillButton->setCooldown(cooldown);

        mRightSkillButton->setCanUse(mStats.canUseSkill(eSkillChoice::right));
    }
}

void eBottomWidget::hideBeltExt() {
    if(!mBeltExt) return;
    mBeltExt->deleteLater();
    mBeltExt = nullptr;
}

void eBottomWidget::showBeltExt() {
    if(mBeltExt) return;
    mBeltExt = new eInventoryBagpackWidget(window());
    mBeltExt->initialize(mClientId,
                         eEquipment::fBeltHPotionSlots,
                         eEquipment::fBeltVPotionSlots - 1,
                         mEq.fBeltHiddenPotions,
                         mEq, eBagpackType::beltExtension,
                         eHoverItemType::regular);
    int x = 0;
    int y = 0;
    mBelt->mapTo(this, x, y);
    addWidget(mBeltExt);
    mBeltExt->move(x, y - mBeltExt->height());
}
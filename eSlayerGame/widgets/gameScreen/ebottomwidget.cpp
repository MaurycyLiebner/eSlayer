#include "ebottomwidget.h"

#include "eskillbutton.h"
#include "eplayerhealthindicator.h"
#include "einventorybagpackwidget.h"
#include "../etexturecheckbutton.h"
#include "../../etext.h"
#include "../ecolors.h"
#include "../../textures/euitextures.h"

#include <eSlayerHelpers/estats.h>
#include <eSlayerHelpers/eequipment.h>

eBottomWidget::eBottomWidget(
    const eStats& stats,
    eEquipment& eq,
    eMainWindow* const window) :
    eWidget(window),
    mStats(stats), mEq(eq) {}

void eBottomWidget::initialize(const eAction& leftSkillA,
                               const int leftSkillId,
                               const eAction& rightSkillA,
                               const int rightSkillId,
                               const eCheckAction& runA,
                               const eCheckAction& portalA,
                               const eCheckAction& invA,
                               const eCheckAction& attrsA,
                               const eCheckAction& skillA,
                               const eCheckAction& partyA) {
    setNoPadding();

    mLeftSkillButton = new eSkillButton(window());
    mLeftSkillButton->initialize(static_cast<int>(eSkillChoice::left));
    mLeftSkillButton->setPressAction(leftSkillA);
    addWidget(mLeftSkillButton);
    mLeftSkillButton->setSkillId(leftSkillId);

    const auto centerWid = new eWidget(window());
    centerWid->setNoPadding();

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

    mBelt = new eInventoryBagpackWidget(window());
    mBelt->initialize(eEquipment::fBeltHPotionSlots,
                      1, mEq.fBeltPotions,
                      mEq, eBagpackType::belt);

    staminaBelt->addWidget(staminaWid);
    staminaBelt->addWidget(mBelt);

    const auto portalButton = new eTextureCheckButton(window());
    portalButton->setTooltip(eText::text(18, 0));
    portalButton->setCheckAction(portalA);
    portalButton->initialize(eUITextures::sPortalIcon,
                             eUITextures::sPortalIcon);
    staminaBelt->addWidget(portalButton);

    const auto& res = resolution();
    const int p = res.tinyPadding();
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

    stackHorizontally();
    fitContent();
    mLeftSkillButton->align(eAlignment::bottom);
    mRightSkillButton->align(eAlignment::bottom);
}

void eBottomWidget::setLeftSkill(const int skillId) {
    mLeftSkillButton->setSkillId(skillId);
}

void eBottomWidget::setRightSkill(const int skillId) {
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
}

void eBottomWidget::hideBeltExt() {
    if(!mBeltExt) return;
    mBeltExt->deleteLater();
    mBeltExt = nullptr;
}

void eBottomWidget::showBeltExt() {
    if(mBeltExt) return;
    mBeltExt = new eInventoryBagpackWidget(window());
    mBeltExt->initialize(eEquipment::fBeltHPotionSlots,
                         eEquipment::fBeltVPotionSlots - 1,
                         mEq.fBeltHiddenPotions,
                         mEq, eBagpackType::beltExtension);
    int x = 0;
    int y = 0;
    mBelt->mapTo(this, x, y);
    addWidget(mBeltExt);
    mBeltExt->move(x, y - mBeltExt->height());
}
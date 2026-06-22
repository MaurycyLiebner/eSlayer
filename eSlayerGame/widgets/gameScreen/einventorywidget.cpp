#include "einventorywidget.h"

#include "../../textures/euitextures.h"
#include "einventorybagpackwidget.h"
#include "eitemplacewidget.h"
#include "ehoverwidget.h"
#include "eweaponswitch.h"
#include "ecoinswidget.h"
#include "estashwidget.h"
#include "ecoinsquestionwidget.h"
#include "egamewidget.h"

#include <eSlayerHelpers/eequipment.h>

bool eInventoryWidget::sBlocked = false;
eInventoryWidget* eInventoryWidget::sInstance = nullptr;

eInventoryWidget::eInventoryWidget(eMainWindow* const window) :
    eBgWidget(window) {
    sInstance = this;
}

eInventoryWidget::~eInventoryWidget() {
    sInstance = nullptr;
}

void eInventoryWidget::initialize(
    eEquipment& eq, const eStats& stats,
    const eHoverItemType htype) {
    mEq = &eq;
    const auto& res = resolution();

    const auto inner = new eWidget(window());
    inner->setNoPadding();

    const auto helmet = new eItemPlaceWidget(window());
    helmet->intialize(eUITextures::sHelmetSlot, eq, stats,
                      &eEquipment::fHelmet,
                      {eItemType::helmet},
                      ePlaceType::helmet, htype);
    mItemPlaces.emplace_back(helmet);

    const auto armor = new eItemPlaceWidget(window());
    armor->intialize(eUITextures::sArmorSlot, eq, stats,
                     &eEquipment::fArmor,
                     {eItemType::armor},
                     ePlaceType::armor, htype);
    mItemPlaces.emplace_back(armor);

    const auto belt = new eItemPlaceWidget(window());
    belt->intialize(eUITextures::sBeltSlot, eq, stats,
                    &eEquipment::fBelt,
                    {eItemType::belt},
                    ePlaceType::belt, htype);
    mItemPlaces.emplace_back(belt);

    const auto boots = new eItemPlaceWidget(window());
    boots->intialize(eUITextures::sBootsSlot, eq, stats,
                     &eEquipment::fBoots,
                     {eItemType::boots},
                     ePlaceType::boots, htype);
    mItemPlaces.emplace_back(boots);

    const auto gloves = new eItemPlaceWidget(window());
    gloves->intialize(eUITextures::sGlovesSlot, eq, stats,
                      &eEquipment::fGloves,
                      {eItemType::gloves},
                      ePlaceType::gloves, htype);
    mItemPlaces.emplace_back(gloves);

    const auto ringL = new eItemPlaceWidget(window());
    ringL->intialize(eUITextures::sRingSlot, eq, stats,
                     &eEquipment::fRingL,
                     {eItemType::ring},
                     ePlaceType::ringL, htype);
    mItemPlaces.emplace_back(ringL);

    const auto ringR = new eItemPlaceWidget(window());
    ringR->intialize(eUITextures::sRingSlot, eq, stats,
                     &eEquipment::fRingR,
                     {eItemType::ring},
                     ePlaceType::ringR, htype);
    mItemPlaces.emplace_back(ringR);

    const auto amulet = new eItemPlaceWidget(window());
    amulet->intialize(eUITextures::sAmuletSlot, eq, stats,
                      &eEquipment::fAmulet,
                      {eItemType::amulet},
                      ePlaceType::amulet, htype);
    mItemPlaces.emplace_back(amulet);

    mWeapon1L = new eItemPlaceWidget(window());
    mWeapon1L->intialize(eUITextures::sWeaponSlot, eq, stats,
                         &eEquipment::fWeapon1L,
                         {eItemType::weapon},
                         ePlaceType::weapon1L, htype);
    mItemPlaces.emplace_back(mWeapon1L);

    mWeapon1R = new eItemPlaceWidget(window());
    mWeapon1R->intialize(eUITextures::sWeaponSlot, eq, stats,
                         &eEquipment::fWeapon1R,
                         {eItemType::weapon,
                          eItemType::shield,
                          eItemType::arrows},
                         ePlaceType::weapon1R, htype);
    mItemPlaces.emplace_back(mWeapon1R);

    mWeapon2L = new eItemPlaceWidget(window());
    mWeapon2L->intialize(eUITextures::sWeaponSlot, eq, stats,
                         &eEquipment::fWeapon2L,
                         {eItemType::weapon},
                         ePlaceType::weapon2L, htype);
    mItemPlaces.emplace_back(mWeapon2L);

    mWeapon2R = new eItemPlaceWidget(window());
    mWeapon2R->intialize(eUITextures::sWeaponSlot, eq, stats,
                         &eEquipment::fWeapon2R,
                         {eItemType::weapon,
                          eItemType::shield,
                          eItemType::arrows},
                         ePlaceType::weapon2R, htype);
    mItemPlaces.emplace_back(mWeapon2R);

    const int p = res.largePadding();

    const auto firstRow = new eWidget(window());
    firstRow->setNoPadding();

    const auto helmetArmor = new eWidget(window());
    helmetArmor->setNoPadding();
    helmetArmor->addWidget(helmet);
    helmetArmor->addWidget(armor);
    helmetArmor->stackVertically(p);
    helmetArmor->fitContent();

    const auto update = [this]() {
        updateWeapons();
    };

    const auto lweaponCont = new eWidget(window());
    lweaponCont->setNoPadding();

    mLWeaponSwitch = new eWeaponSwitch(window());
    mLWeaponSwitch->initialize(mEq, update);
    lweaponCont->addWidget(mLWeaponSwitch);

    const auto lweapon = new eWidget(window());
    lweapon->setNoPadding();
    lweapon->addWidget(mWeapon1L);
    lweapon->addWidget(mWeapon2L);
    lweapon->fitContent();

    lweaponCont->addWidget(lweapon);
    lweaponCont->stackVertically();
    lweaponCont->fitContent();

    const auto rweaponCont = new eWidget(window());
    rweaponCont->setNoPadding();

    mRWeaponSwitch = new eWeaponSwitch(window());
    mRWeaponSwitch->initialize(mEq, update);
    rweaponCont->addWidget(mRWeaponSwitch);

    const auto rweapon = new eWidget(window());
    rweapon->setNoPadding();
    rweapon->addWidget(mWeapon1R);
    rweapon->addWidget(mWeapon2R);
    rweapon->fitContent();

    rweaponCont->addWidget(rweapon);
    rweaponCont->stackVertically();
    rweaponCont->fitContent();

    firstRow->addWidget(lweaponCont);
    firstRow->addWidget(helmetArmor);
    firstRow->addWidget(amulet);
    firstRow->addWidget(rweaponCont);
    firstRow->stackHorizontally(p);
    firstRow->fitContent();
    inner->addWidget(firstRow);

    const auto secondRow = new eWidget(window());
    secondRow->setNoPadding();

    secondRow->addWidget(gloves);
    secondRow->addWidget(ringL);
    secondRow->addWidget(belt);
    secondRow->addWidget(ringR);
    secondRow->addWidget(boots);
    secondRow->stackHorizontally(p);
    secondRow->fitContent();
    inner->addWidget(secondRow);

    mBagpack = new eInventoryBagpackWidget(window());
    mBagpack->initialize(eEquipment::fInventoryWidth,
                         eEquipment::fInventoryHeight,
                         eq.fInventory,
                         eq, eBagpackType::inventory);
    inner->addWidget(mBagpack);

    mCoins = new eCoinsWidget(window());
    const bool stash = eStashWidget::sInstance;
    const auto action = [this, stash, &eq]() {
        const auto q = new eCoinsQuestionWidget(window());
        const int s = stash ? 10 : 9;
        const auto goldA = [stash, &eq](const int count) {
            eq.fInventoryGold -= count;
            if(stash) {
                eq.fStashGold += count;
            } else {
                eGameWidget::sDropGold(count);
            }
            eEquipmentAction a;
            a.fType = eEquipmentActionType::gold;
            a.fStashGold = eq.fStashGold;
            a.fInvGold = eq.fInventoryGold;
            eGameWidget::sSendEqAction(a);
        };
        q->initialize(goldA, s, eq.fInventoryGold);
        addWidget(q);
        q->align(eAlignment::center);
    };
    mCoins->initialize(eq.fInventoryGold,
                       action, stash ? 7 : 6);
    inner->addWidget(mCoins);

    inner->stackVertically(p);
    inner->fitContent();

    firstRow->align(eAlignment::hcenter);
    secondRow->align(eAlignment::hcenter);
    mBagpack->align(eAlignment::hcenter);
    mCoins->align(eAlignment::hcenter);

    setup(inner);
}

bool eInventoryWidget::mousePressEvent(const eMouseEvent& e) {
    return true;
}

bool eInventoryWidget::dropItem() {
    if(sBlocked) return false;
    const bool b = mBagpack->dropItem();
    if(b) return true;
    for(const auto w : mItemPlaces) {
        if(!w->visible()) continue;
        if(!w->hovered()) continue;
        const bool r = w->dropItem();
        if(r) return true;
    }
    return false;
}

void eInventoryWidget::updateWeapons() {
    mLWeaponSwitch->updateChecked();
    mRWeaponSwitch->updateChecked();
    mWeapon1L->setVisible(mEq->fWeapons1);
    mWeapon1R->setVisible(mEq->fWeapons1);
    mWeapon2L->setVisible(!mEq->fWeapons1);
    mWeapon2R->setVisible(!mEq->fWeapons1);
    if(mWeapon1L->hovered()) {
        if(mEq->fWeapons1) {
            mWeapon1L->setHoverItem();
        } else {
            mWeapon2L->setHoverItem();
        }
    } else if(mWeapon1R->hovered()) {
        if(mEq->fWeapons1) {
            mWeapon1R->setHoverItem();
        } else {
            mWeapon2R->setHoverItem();
        }
    }
}

void eInventoryWidget::paintEvent(ePainter& p) {
    eBgWidget::paintEvent(p);
    mCoins->setCount(mEq->fInventoryGold);
}

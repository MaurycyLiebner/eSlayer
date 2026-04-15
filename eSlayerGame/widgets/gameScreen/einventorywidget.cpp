#include "einventorywidget.h"

#include "../../textures/euitextures.h"
#include "einventorybagpackwidget.h"
#include "eitemplacewidget.h"
#include "eitemdragwidget.h"
#include "eweaponswitch.h"

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

void eInventoryWidget::initialize(eEquipment& eq, const eStats& stats) {
    mEq = &eq;
    const auto& res = resolution();

    const auto inner = new eWidget(window());
    inner->setNoPadding();

    const auto helmet = new eItemPlaceWidget(window());
    helmet->intialize(eUITextures::sHelmetSlot, eq, stats,
                      &eEquipment::fHelmet,
                      {eItemType::helmet});
    mItemPalces.emplace_back(helmet);

    const auto armor = new eItemPlaceWidget(window());
    armor->intialize(eUITextures::sArmorSlot, eq, stats,
                     &eEquipment::fArmor,
                     {eItemType::armor});
    mItemPalces.emplace_back(armor);

    const auto belt = new eItemPlaceWidget(window());
    belt->intialize(eUITextures::sBeltSlot, eq, stats,
                    &eEquipment::fBelt,
                    {eItemType::belt});
    mItemPalces.emplace_back(belt);

    const auto boots = new eItemPlaceWidget(window());
    boots->intialize(eUITextures::sBootsSlot, eq, stats,
                     &eEquipment::fBoots,
                     {eItemType::boots});
    mItemPalces.emplace_back(boots);

    const auto gloves = new eItemPlaceWidget(window());
    gloves->intialize(eUITextures::sGlovesSlot, eq, stats,
                      &eEquipment::fGloves,
                      {eItemType::gloves});
    mItemPalces.emplace_back(gloves);

    const auto ringL = new eItemPlaceWidget(window());
    ringL->intialize(eUITextures::sRingSlot, eq, stats,
                     &eEquipment::fRingL,
                     {eItemType::ring});
    mItemPalces.emplace_back(ringL);

    const auto ringR = new eItemPlaceWidget(window());
    ringR->intialize(eUITextures::sRingSlot, eq, stats,
                     &eEquipment::fRingR,
                     {eItemType::ring});
    mItemPalces.emplace_back(ringR);

    const auto amulet = new eItemPlaceWidget(window());
    amulet->intialize(eUITextures::sAmuletSlot, eq, stats,
                      &eEquipment::fAmulet,
                      {eItemType::amulet});
    mItemPalces.emplace_back(amulet);

    mWeapon1L = new eItemPlaceWidget(window());
    mWeapon1L->intialize(eUITextures::sWeaponSlot, eq, stats,
                         &eEquipment::fWeapon1L,
                         {eItemType::weapon});
    mItemPalces.emplace_back(mWeapon1L);

    mWeapon1R = new eItemPlaceWidget(window());
    mWeapon1R->intialize(eUITextures::sWeaponSlot, eq, stats,
                         &eEquipment::fWeapon1R,
                         {eItemType::weapon,
                          eItemType::shield,
                          eItemType::arrows});
    mItemPalces.emplace_back(mWeapon1R);

    mWeapon2L = new eItemPlaceWidget(window());
    mWeapon2L->intialize(eUITextures::sWeaponSlot, eq, stats,
                         &eEquipment::fWeapon2L,
                         {eItemType::weapon});
    mItemPalces.emplace_back(mWeapon2L);

    mWeapon2R = new eItemPlaceWidget(window());
    mWeapon2R->intialize(eUITextures::sWeaponSlot, eq, stats,
                         &eEquipment::fWeapon2R,
                         {eItemType::weapon,
                          eItemType::shield,
                          eItemType::arrows});
    mItemPalces.emplace_back(mWeapon2R);

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
                         eq, stats);
    inner->addWidget(mBagpack);

    inner->stackVertically(p);
    inner->fitContent();

    firstRow->align(eAlignment::hcenter);
    secondRow->align(eAlignment::hcenter);
    mBagpack->align(eAlignment::hcenter);

    setup(inner);
}

bool eInventoryWidget::mousePressEvent(const eMouseEvent& e) {
    return true;
}

bool eInventoryWidget::dropItem() {
    if(sBlocked) return false;
    const bool b = mBagpack->dropItem();
    if(b) return true;
    for(const auto w : mItemPalces) {
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
            eItemDragWidget::sSetHoverItem(mEq->fWeapon1L);
        } else {
            eItemDragWidget::sSetHoverItem(mEq->fWeapon2L);
        }
    } else if(mWeapon1R->hovered()) {
        if(mEq->fWeapons1) {
            eItemDragWidget::sSetHoverItem(mEq->fWeapon1R);
        } else {
            eItemDragWidget::sSetHoverItem(mEq->fWeapon2R);
        }
    }
}

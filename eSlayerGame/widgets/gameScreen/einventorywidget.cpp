#include "einventorywidget.h"

#include "einventorybagpackwidget.h"
#include "eitemplacewidget.h"
#include "eweaponswitch.h"

#include <eSlayerHelpers/eequipment.h>

bool eInventoryWidget::sBlocked = false;
eItemDragWidget* eInventoryWidget::sDragWidget = nullptr;

void eInventoryWidget::initialize(
    eEquipment* const eq) {
    mEq = eq;
    const auto& res = resolution();
    const int dim = res.inventoryDim();

    const auto helmet = new eItemPlaceWidget(window());
    helmet->intialize(2, 2, dim, mEq,
                      &eEquipment::fHelmet,
                      {eItemType::helmet});
    mItemPalces.emplace_back(helmet);

    const auto armor = new eItemPlaceWidget(window());
    armor->intialize(2, 3, dim, mEq,
                      &eEquipment::fArmor,
                      {eItemType::armor});
    mItemPalces.emplace_back(armor);

    const auto belt = new eItemPlaceWidget(window());
    belt->intialize(2, 1, dim, mEq,
                     &eEquipment::fBelt,
                     {eItemType::belt});
    mItemPalces.emplace_back(belt);

    const auto boots = new eItemPlaceWidget(window());
    boots->intialize(2, 2, dim, mEq,
                     &eEquipment::fBoots,
                     {eItemType::boots});
    mItemPalces.emplace_back(boots);

    const auto gloves = new eItemPlaceWidget(window());
    gloves->intialize(2, 2, dim, mEq,
                      &eEquipment::fGloves,
                      {eItemType::gloves});
    mItemPalces.emplace_back(gloves);

    const auto ringL = new eItemPlaceWidget(window());
    ringL->intialize(1, 1, dim, mEq,
                     &eEquipment::fRingL,
                     {eItemType::ring});
    mItemPalces.emplace_back(ringL);

    const auto ringR = new eItemPlaceWidget(window());
    ringR->intialize(1, 1, dim, mEq,
                     &eEquipment::fRingR,
                     {eItemType::ring});
    mItemPalces.emplace_back(ringR);

    const auto amulet = new eItemPlaceWidget(window());
    amulet->intialize(1, 1, dim, mEq,
                      &eEquipment::fAmulet,
                      {eItemType::amulet});
    mItemPalces.emplace_back(amulet);

    mWeapon1L = new eItemPlaceWidget(window());
    mWeapon1L->intialize(2, 4, dim, mEq,
                      &eEquipment::fWeapon1L,
                      {eItemType::weapon});
    mItemPalces.emplace_back(mWeapon1L);

    mWeapon1R = new eItemPlaceWidget(window());
    mWeapon1R->intialize(2, 4, dim, mEq,
                        &eEquipment::fWeapon1R,
                        {eItemType::weapon,
                         eItemType::shield,
                         eItemType::arrows});
    mItemPalces.emplace_back(mWeapon1R);

    mWeapon2L = new eItemPlaceWidget(window());
    mWeapon2L->intialize(2, 4, dim, mEq,
                        &eEquipment::fWeapon2L,
                        {eItemType::weapon});
    mItemPalces.emplace_back(mWeapon2L);

    mWeapon2R = new eItemPlaceWidget(window());
    mWeapon2R->intialize(2, 4, dim, mEq,
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
    addWidget(firstRow);

    const auto secondRow = new eWidget(window());
    secondRow->setNoPadding();

    secondRow->addWidget(gloves);
    secondRow->addWidget(ringL);
    secondRow->addWidget(belt);
    secondRow->addWidget(ringR);
    secondRow->addWidget(boots);
    secondRow->stackHorizontally(p);
    secondRow->fitContent();
    addWidget(secondRow);

    mBagpack = new eInventoryBagpackWidget(window());
    mBagpack->initialize(mEq, dim);
    addWidget(mBagpack);

    layoutVertically();

    firstRow->align(eAlignment::hcenter);
    secondRow->align(eAlignment::hcenter);
    mBagpack->align(eAlignment::hcenter);

    updateWeapons();
}

void eInventoryWidget::paintEvent(ePainter& p) {

}

bool eInventoryWidget::dropItem(const SDL_Point& pos) {
    if(sBlocked) return false;
    const auto bmpos = mBagpack->mousePos();
    const bool b = mBagpack->dropItem(bmpos);
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
}

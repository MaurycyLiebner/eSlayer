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
#include <eSlayerHelpers/evectorhelpers.h>

bool eInventoryWidget::sBlocked = false;

void eInventoryWidget::initialize(
    eEquipment& eq, const eStats& stats,
    const eHoverItemType htype,
    const std::vector<ePlaceType>& places,
    eItem* dragged) {
    mEq = &eq;
    if(!dragged) {
        dragged = &mEq->fDragged;
    }
    const auto& res = resolution();

    const auto inner = new eWidget(window());
    inner->setNoPadding();

    eItemPlaceWidget* helmet = nullptr;
    if(eVectorHelpers::contains(places, ePlaceType::helmet)) {
        helmet = new eItemPlaceWidget(window());
        helmet->intialize(eUITextures::sHelmetSlot, eq, &stats,
                          &eEquipment::fHelmet,
                          ePlaceType::helmet, htype,
                          dragged);
        mItemPlaces.emplace_back(helmet);
    }

    eItemPlaceWidget* armor = nullptr;
    if(eVectorHelpers::contains(places, ePlaceType::armor)) {
        armor = new eItemPlaceWidget(window());
        armor->intialize(eUITextures::sArmorSlot, eq, &stats,
                         &eEquipment::fArmor,
                         ePlaceType::armor, htype,
                         dragged);
        mItemPlaces.emplace_back(armor);
    }

    eItemPlaceWidget* belt = nullptr;
    if(eVectorHelpers::contains(places, ePlaceType::belt)) {
        belt = new eItemPlaceWidget(window());
        belt->intialize(eUITextures::sBeltSlot, eq, &stats,
                        &eEquipment::fBelt,
                        ePlaceType::belt, htype,
                        dragged);
        mItemPlaces.emplace_back(belt);
    }

    eItemPlaceWidget* boots = nullptr;
    if(eVectorHelpers::contains(places, ePlaceType::boots)) {
        boots = new eItemPlaceWidget(window());
        boots->intialize(eUITextures::sBootsSlot, eq, &stats,
                         &eEquipment::fBoots,
                         ePlaceType::boots, htype,
                         dragged);
        mItemPlaces.emplace_back(boots);
    }

    eItemPlaceWidget* gloves = nullptr;
    if(eVectorHelpers::contains(places, ePlaceType::gloves)) {
        gloves = new eItemPlaceWidget(window());
        gloves->intialize(eUITextures::sGlovesSlot, eq, &stats,
                          &eEquipment::fGloves,
                          ePlaceType::gloves, htype,
                          dragged);
        mItemPlaces.emplace_back(gloves);
    }

    eItemPlaceWidget* ringL = nullptr;
    if(eVectorHelpers::contains(places, ePlaceType::ringL)) {
        ringL = new eItemPlaceWidget(window());
        ringL->intialize(eUITextures::sRingSlot, eq, &stats,
                         &eEquipment::fRingL,
                         ePlaceType::ringL, htype,
                         dragged);
        mItemPlaces.emplace_back(ringL);
    }

    eItemPlaceWidget* ringR = nullptr;
    if(eVectorHelpers::contains(places, ePlaceType::ringR)) {
        ringR = new eItemPlaceWidget(window());
        ringR->intialize(eUITextures::sRingSlot, eq, &stats,
                         &eEquipment::fRingR,
                         ePlaceType::ringR, htype,
                         dragged);
        mItemPlaces.emplace_back(ringR);
    }

    eItemPlaceWidget* amulet = nullptr;
    if(eVectorHelpers::contains(places, ePlaceType::amulet)) {
        amulet = new eItemPlaceWidget(window());
        amulet->intialize(eUITextures::sAmuletSlot, eq, &stats,
                          &eEquipment::fAmulet,
                          ePlaceType::amulet, htype,
                          dragged);
        mItemPlaces.emplace_back(amulet);
    }

    if(eVectorHelpers::contains(places, ePlaceType::weapon1L)) {
        mWeapon1L = new eItemPlaceWidget(window());
        mWeapon1L->intialize(eUITextures::sWeaponSlot, eq, &stats,
                             &eEquipment::fWeapon1L,
                             ePlaceType::weapon1L, htype,
                             dragged);
        mItemPlaces.emplace_back(mWeapon1L);
    }

    if(eVectorHelpers::contains(places, ePlaceType::weapon1R)) {
        mWeapon1R = new eItemPlaceWidget(window());
        mWeapon1R->intialize(eUITextures::sWeaponSlot, eq, &stats,
                             &eEquipment::fWeapon1R,
                             ePlaceType::weapon1R, htype,
                             dragged);
        mItemPlaces.emplace_back(mWeapon1R);
    }

    if(eVectorHelpers::contains(places, ePlaceType::weapon2L)) {
        mWeapon2L = new eItemPlaceWidget(window());
        mWeapon2L->intialize(eUITextures::sWeaponSlot, eq, &stats,
                             &eEquipment::fWeapon2L,
                             ePlaceType::weapon2L, htype,
                             dragged);
        mItemPlaces.emplace_back(mWeapon2L);
    }

    if(eVectorHelpers::contains(places, ePlaceType::weapon2R)) {
        mWeapon2R = new eItemPlaceWidget(window());
        mWeapon2R->intialize(eUITextures::sWeaponSlot, eq, &stats,
                             &eEquipment::fWeapon2R,
                             ePlaceType::weapon2R, htype,
                             dragged);
        mItemPlaces.emplace_back(mWeapon2R);
    }

    const int p = res.largePadding();

    const auto firstRow = new eWidget(window());
    firstRow->setNoPadding();

    const auto helmetArmor = new eWidget(window());
    helmetArmor->setNoPadding();
    if(helmet) helmetArmor->addWidget(helmet);
    if(armor) helmetArmor->addWidget(armor);
    helmetArmor->stackVertically(p);
    helmetArmor->fitContent();

    const auto update = [this]() {
        updateWeapons();
    };

    const auto lweaponCont = new eWidget(window());
    lweaponCont->setNoPadding();

    const bool switchEnabled = (mWeapon1L && mWeapon2L) ||
                               (mWeapon1R && mWeapon2R);
    if(switchEnabled) {
        mLWeaponSwitch = new eWeaponSwitch(window());
        mLWeaponSwitch->initialize(mEq, update);
        lweaponCont->addWidget(mLWeaponSwitch);
    }

    const auto lweapon = new eWidget(window());
    lweapon->setNoPadding();
    if(mWeapon1L) lweapon->addWidget(mWeapon1L);
    if(mWeapon2L) lweapon->addWidget(mWeapon2L);
    lweapon->fitContent();

    lweaponCont->addWidget(lweapon);
    lweaponCont->stackVertically();
    lweaponCont->fitContent();

    const auto rweaponCont = new eWidget(window());
    rweaponCont->setNoPadding();

    if(switchEnabled) {
        mRWeaponSwitch = new eWeaponSwitch(window());
        mRWeaponSwitch->initialize(mEq, update);
        rweaponCont->addWidget(mRWeaponSwitch);
    }

    const auto rweapon = new eWidget(window());
    rweapon->setNoPadding();
    if(mWeapon1R) rweapon->addWidget(mWeapon1R);
    if(mWeapon2R) rweapon->addWidget(mWeapon2R);
    rweapon->fitContent();

    rweaponCont->addWidget(rweapon);
    rweaponCont->stackVertically();
    rweaponCont->fitContent();

    firstRow->addWidget(lweaponCont);
    firstRow->addWidget(helmetArmor);
    if(amulet) firstRow->addWidget(amulet);
    firstRow->addWidget(rweaponCont);
    firstRow->stackHorizontally(p);
    firstRow->fitContent();
    inner->addWidget(firstRow);

    const auto secondRow = new eWidget(window());
    secondRow->setNoPadding();

    if(gloves) secondRow->addWidget(gloves);
    if(ringL) secondRow->addWidget(ringL);
    if(belt) secondRow->addWidget(belt);
    if(ringR) secondRow->addWidget(ringR);
    if(boots) secondRow->addWidget(boots);
    secondRow->stackHorizontally(p);
    secondRow->fitContent();
    inner->addWidget(secondRow);

    if(eVectorHelpers::contains(places, ePlaceType::inventory)) {
        mBagpack = new eInventoryBagpackWidget(window());
        mBagpack->initialize(eEquipment::fInventoryWidth,
                             eEquipment::fInventoryHeight,
                             eq.fInventory,
                             eq, eBagpackType::inventory,
                             htype, dragged);
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
    }

    inner->stackVertically(p);
    inner->fitContent();

    firstRow->align(eAlignment::hcenter);
    secondRow->align(eAlignment::hcenter);
    if(mBagpack) mBagpack->align(eAlignment::hcenter);
    if(mCoins) mCoins->align(eAlignment::hcenter);

    setup(inner);

    updateWeapons();
}

bool eInventoryWidget::dropItem() {
    if(sBlocked) return false;
    const bool b = mBagpack && mBagpack->dropItem();
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
    if(mLWeaponSwitch) mLWeaponSwitch->updateChecked();
    if(mRWeaponSwitch) mRWeaponSwitch->updateChecked();
    if(mWeapon1L) mWeapon1L->setVisible(mEq->fWeapons1);
    if(mWeapon1R) mWeapon1R->setVisible(mEq->fWeapons1);
    if(mWeapon2L) mWeapon2L->setVisible(!mEq->fWeapons1);
    if(mWeapon2R) mWeapon2R->setVisible(!mEq->fWeapons1);
    if((mWeapon1L && mWeapon1L->hovered()) ||
       (mWeapon2L && mWeapon2L->hovered())) {
        if(mEq->fWeapons1) {
            if(mWeapon1L) mWeapon1L->setHoverItem();
        } else {
            if(mWeapon2L) mWeapon2L->setHoverItem();
        }
    } else if((mWeapon1R && mWeapon1R->hovered()) ||
              (mWeapon2R && mWeapon2R->hovered())) {
        if(mEq->fWeapons1) {
            if(mWeapon1R) mWeapon1R->setHoverItem();
        } else {
            if(mWeapon2R) mWeapon2R->setHoverItem();
        }
    }
}

void eInventoryWidget::paintEvent(ePainter& p) {
    eBgWidget::paintEvent(p);
    if(mCoins) mCoins->setCount(mEq->fInventoryGold);
}

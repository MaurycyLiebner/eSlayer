#include "eSlayerHelpers/eequipmentaction.h"

#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/epacket.h"

bool eEquipmentAction::add(eEquipment& eq) const {
    return add(eq, fAddItem, fPlace);
}

bool eEquipmentAction::add(
    eEquipment& eq,
    const eItem& item,
    const eEquipmentPlace place) {
    const auto placeOnPlace = [&](eItem eEquipment::*it) {
        auto& dst = eq.*it;
        if(dst.fType != eItemType::none) return false;
        dst = item;
        return true;
    };
    const auto placeInInv = [&](eInventoryItems eEquipment::*it) {
        auto& dst = eq.*it;
        return dst.tryAdd(item, place.fX, place.fY);
    };
    switch(place.fType) {
    case ePlaceType::none:
        return false;
    case ePlaceType::boots:
        return placeOnPlace(&eEquipment::fBoots);
    case ePlaceType::gloves:
        return placeOnPlace(&eEquipment::fGloves);
    case ePlaceType::helmet:
        return placeOnPlace(&eEquipment::fHelmet);
    case ePlaceType::armor:
        return placeOnPlace(&eEquipment::fArmor);
    case ePlaceType::belt:
        return placeOnPlace(&eEquipment::fBelt);
    case ePlaceType::ringL:
        return placeOnPlace(&eEquipment::fRingL);
    case ePlaceType::ringR:
        return placeOnPlace(&eEquipment::fRingR);
    case ePlaceType::amulet:
        return placeOnPlace(&eEquipment::fAmulet);
    case ePlaceType::weapon1L:
        return placeOnPlace(&eEquipment::fWeapon1L);
    case ePlaceType::weapon1R:
        return placeOnPlace(&eEquipment::fWeapon1R);
    case ePlaceType::weapon2L:
        return placeOnPlace(&eEquipment::fWeapon2L);
    case ePlaceType::weapon2R:
        return placeOnPlace(&eEquipment::fWeapon2R);
    case ePlaceType::dragged:
        return placeOnPlace(&eEquipment::fDragged);
    case ePlaceType::inventory:
        return placeInInv(&eEquipment::fInventory);
    case ePlaceType::stash:
        return placeInInv(&eEquipment::fStash);
    case ePlaceType::beltPotions:
        return placeInInv(&eEquipment::fBeltPotions);
    case ePlaceType::beltHiddenPotions:
        return placeInInv(&eEquipment::fBeltHiddenPotions);
    case ePlaceType::temporary:
        return placeOnPlace(&eEquipment::fTemporary);
    }
    return false;
}

bool eEquipmentAction::drag(eEquipment& eq) const {
    auto& eqD = eq.fDragged;
    if(eqD.fType != eItemType::none) return false;
    eqD = eq.take(fItemId1);
    return true;
}

bool eEquipmentAction::switchDrag(eEquipment& eq) const {
    auto& eqD = eq.fDragged;
    if(eqD.fType == eItemType::none) return false;
    auto tmp = eqD;
    eqD = eq.take(fItemId1);
    return add(eq, tmp, fPlace);
}

bool eEquipmentAction::drop(eEquipment& eq) const {
    auto& eqD = eq.fDragged;
    if(eqD.fType == eItemType::none) return false;
    const bool r = add(eq, eqD, fPlace);
    if(!r) return false;
    eqD = eItem();
    return true;
}

bool eEquipmentAction::dragAndDrop(eEquipment& eq) const {
    const auto i = eq.take(fItemId1);
    if(i.fType == eItemType::none) return false;
    const bool r = add(eq, i, fPlace);
    return r;
}

bool eEquipmentAction::insertJewel(eEquipment& eq) const {
    const auto jewelId = fItemId1;
    const auto targetId = fItemId2;
    return eq.insertJewel(jewelId, targetId);
}

bool eEquipmentAction::gold(eEquipment& eq) const {
    eq.fInventoryGold = fInvGold;
    eq.fStashGold = fStashGold;
    return true;
}

bool eEquipmentAction::switchWeapons(
    eEquipment& eq) const {
    eq.fWeapons1 = fWeapons1;
    return true;
}

bool eEquipmentAction::apply(eEquipment& eq) const {
    switch(fType) {
    case eEquipmentActionType::add:
        return add(eq);
    case eEquipmentActionType::drag:
        return drag(eq);
    case eEquipmentActionType::switchDrag:
        return switchDrag(eq);
    case eEquipmentActionType::dragAndDrop:
        return dragAndDrop(eq);
    case eEquipmentActionType::insertJewel:
        return insertJewel(eq);
    case eEquipmentActionType::drop:
        return drop(eq);
    case eEquipmentActionType::gold:
        return gold(eq);
    case eEquipmentActionType::switchWeapons:
        return switchWeapons(eq);
    case eEquipmentActionType::none:
        return false;
    }
    return false;
}

void eEquipmentAction::read(ePacket& p) {
    p >> fType;
    p >> fPlace;
    fAddItem.read(p);
    p >> fItemId1;
    p >> fItemId2;

    p >> fInvGold;
    p >> fStashGold;

    p >> fWeapons1;
}

void eEquipmentAction::write(ePacket& p) const {
    p << fType;
    p << fPlace;
    fAddItem.write(p);
    p << fItemId1;
    p << fItemId2;

    p << fInvGold;
    p << fStashGold;

    p << fWeapons1;
}
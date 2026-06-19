#include "eSlayerHelpers/eequipmentaction.h"

#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/epacket.h"

bool eEquipmentAction::add(eEquipment& eq) const {
    const auto placeOnPlace = [&](eItem eEquipment::*it) {
        auto& dst = eq.*it;
        if(dst.fType != eItemType::none) return false;
        dst = fItem;
        return true;
    };
    const auto placeInInv = [&](eInventoryItems eEquipment::*it) {
        auto& dst = eq.*it;
        return dst.tryAdd(fItem, fPlace.fX, fPlace.fY);
    };
    switch(fPlace.fType) {
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
    }
    return false;
}

bool eEquipmentAction::apply(eEquipment& eq) const {
    switch(fType) {
    case eEquipmentActionType::add:
        return add(eq);
    case eEquipmentActionType::none:
        return false;
    }
    return false;
}

void eEquipmentAction::read(ePacket& p) {
    p >> fType;
    p >> fPlace;
    fItem.read(p);
}

void eEquipmentAction::write(ePacket& p) const {
    p << fType;
    p << fPlace;
    fItem.write(p);
}
#include "eSlayerHelpers/eequipment.h"

#include "eSlayerHelpers/epacket.h"

void eInventoryItem::read(ePacket& p) {
    p >> fItem;
    p >> fX;
    p >> fY;
    p >> fW;
    p >> fH;
}

void eInventoryItem::write(ePacket& p) const {
    p << fItem;
    p << fX;
    p << fY;
    p << fW;
    p << fH;
}

eItem eEquipment::get(const int itemId) const {
    for(const auto& it : {fBoots,
                          fGloves,
                          fHelmet,
                          fArmor,
                          fBelt,
                          fRing,
                          fAmulet,
                          fWeapon1L,
                          fWeapon1R,
                          fWeapon2L,
                          fWeapon2R,
                          fDragged}) {
        if(it.fItemId == itemId) return it;
    }
    for(const auto& it : fInventory) {
        const auto& item = it.fItem;
        if(item.fItemId == itemId) return item;
    }
    return eItem{0, 0, eItemType::none};
}

void eEquipment::read(ePacket& p) {
    fBoots.read(p);
    fGloves.read(p);
    fHelmet.read(p);
    fArmor.read(p);
    fBelt.read(p);
    fRing.read(p);
    fAmulet.read(p);
    fWeapon1L.read(p);
    fWeapon1R.read(p);
    fWeapon2L.read(p);
    fWeapon2R.read(p);
    p >> fWeapons1;
    fDragged.read(p);

    uint16_t nitems;
    p >> nitems;
    for(int i = 0; i < nitems; i++) {
        auto& item = fInventory.emplace_back();
        item.read(p);
    }
}

void eEquipment::write(ePacket& p) const {
    fBoots.write(p);
    fGloves.write(p);
    fHelmet.write(p);
    fArmor.write(p);
    fBelt.write(p);
    fRing.write(p);
    fAmulet.write(p);
    fWeapon1L.write(p);
    fWeapon1R.write(p);
    fWeapon2L.write(p);
    fWeapon2R.write(p);
    p << fWeapons1;
    fDragged.write(p);

    const uint16_t nitems = fInventory.size();
    p << nitems;
    for(const auto& item : fInventory) {
        item.write(p);
    }
}

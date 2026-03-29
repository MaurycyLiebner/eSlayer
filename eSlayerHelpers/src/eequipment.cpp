#include "eSlayerHelpers/eequipment.h"

#include "eSlayerHelpers/eitemsdata.h"
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
                          fRingL,
                          fRingR,
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

bool eEquipment::add(const eItem& item) {
    const auto tryAdd = [&](eItem& dst, const eItemType type) {
        if(dst.fType != eItemType::none) return false;
        if(item.fType != type) return false;
        dst = item;
        return true;
    };

    bool r = tryAdd(fBoots, eItemType::boots);
    if(r) return true;
    r = tryAdd(fGloves, eItemType::gloves);
    if(r) return true;
    r = tryAdd(fHelmet, eItemType::helmet);
    if(r) return true;
    r = tryAdd(fArmor, eItemType::armor);
    if(r) return true;
    r = tryAdd(fBelt, eItemType::belt);
    if(r) return true;
    r = tryAdd(fRingL, eItemType::ring);
    if(r) return true;
    r = tryAdd(fRingR, eItemType::ring);
    if(r) return true;
    r = tryAdd(fAmulet, eItemType::amulet);
    if(r) return true;
    r = tryAdd(fWeapon1L, eItemType::weapon);
    if(r) return true;
    r = tryAdd(fWeapon1R, eItemType::weapon);
    if(r) return true;
    r = tryAdd(fWeapon2L, eItemType::weapon);
    if(r) return true;
    r = tryAdd(fWeapon2R, eItemType::weapon);
    if(r) return true;
    r = tryAdd(fWeapon1R, eItemType::shield);
    if(r) return true;
    r = tryAdd(fWeapon2R, eItemType::shield);
    if(r) return true;
    r = tryAdd(fWeapon1R, eItemType::arrows);
    if(r) return true;
    r = tryAdd(fWeapon2R, eItemType::arrows);
    if(r) return true;
    const auto& itemData = eItemsData::get(item.fDataId);
    const int w = itemData.fWidth;
    const int h = itemData.fHeight;
    for(int x = 0; x <= fInventoryWidth - w; x++) {
        for(int y = 0; y <= fInventoryHeight - h; y++) {
            bool overlap = false;
            for(const auto& it : fInventory) {
                const bool dontOverlap = x >= it.fX + it.fW ||
                                         x + w <= it.fX ||
                                         y >= it.fY + it.fH ||
                                         y + h <= it.fY;
                if(dontOverlap) continue;
                overlap = true;
                break;
            }
            if(!overlap) {
                auto& iitem = fInventory.emplace_back();
                iitem.fItem = item;
                iitem.fX = x;
                iitem.fY = y;
                iitem.fW = w;
                iitem.fH = h;
                return true;
            }
        }
    }
    return false;
}

void eEquipment::read(ePacket& p) {
    fBoots.read(p);
    fGloves.read(p);
    fHelmet.read(p);
    fArmor.read(p);
    fBelt.read(p);
    fRingL.read(p);
    fRingR.read(p);
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
    fRingL.write(p);
    fRingR.write(p);
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

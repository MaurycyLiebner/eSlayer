#include "eSlayerHelpers/eequipment.h"

#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/eweapontype.h"

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
    const auto tryAdd = [&](eItem& dst) {
        if(dst.fType != eItemType::none) return false;
        const bool r = canPlace(item, dst);
        if(!r) return false;
        dst = item;
        return true;
    };

    bool r = tryAdd(fBoots);
    if(r) return true;
    r = tryAdd(fGloves);
    if(r) return true;
    r = tryAdd(fHelmet);
    if(r) return true;
    r = tryAdd(fArmor);
    if(r) return true;
    r = tryAdd(fBelt);
    if(r) return true;
    r = tryAdd(fRingL);
    if(r) return true;
    r = tryAdd(fRingR);
    if(r) return true;
    r = tryAdd(fAmulet);
    if(r) return true;
    r = tryAdd(fWeapon1L);
    if(r) return true;
    r = tryAdd(fWeapon1R);
    if(r) return true;
    r = tryAdd(fWeapon2L);
    if(r) return true;
    r = tryAdd(fWeapon2R);
    if(r) return true;
    r = tryAdd(fWeapon1R);
    if(r) return true;
    r = tryAdd(fWeapon2R);
    if(r) return true;
    r = tryAdd(fWeapon1R);
    if(r) return true;
    r = tryAdd(fWeapon2R);
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

bool eEquipment::canPlace(const eItem& item, const eItem& dst) {
    eItemType type = eItemType::none;;
    if(&dst == &fBoots) {
        type = eItemType::boots;
    } else if(&dst == &fGloves) {
        type = eItemType::gloves;
    } else if(&dst == &fHelmet) {
        type = eItemType::helmet;
    } else if(&dst == &fArmor) {
        type = eItemType::armor;
    } else if(&dst == &fBelt) {
        type = eItemType::belt;
    } else if(&dst == &fRingL || &dst == &fRingR) {
        type = eItemType::ring;
    } else if(&dst == &fAmulet) {
        type = eItemType::amulet;
    } else {
        if(&dst == &fWeapon1L || &dst == &fWeapon2L) {
            if(item.fType != eItemType::weapon) return false;
            const auto subtype = static_cast<eWeaponSubtype>(item.fSubType);
            if(subtype == eWeaponSubtype::bow) {
                if(&dst == &fWeapon1L) {
                    return fWeapon1R.fType == eItemType::none ||
                           fWeapon1R.fType == eItemType::arrows;
                } else { // if(&dst == &fWeapon2L) {
                    return fWeapon2R.fType == eItemType::none ||
                           fWeapon2R.fType == eItemType::arrows;
                }
            } else if(subtype == eWeaponSubtype::pike) {
                if(&dst == &fWeapon1L) {
                    return fWeapon1R.fType == eItemType::none;
                } else { // if(&dst == &fWeapon2L) {
                    return fWeapon2R.fType == eItemType::none;
                }
            } else {
                if(&dst == &fWeapon1L) {
                    return fWeapon1R.fType == eItemType::weapon ||
                           fWeapon1R.fType == eItemType::none;
                } else { // if(&dst == &fWeapon2L) {
                    return fWeapon2R.fType == eItemType::weapon ||
                           fWeapon2R.fType == eItemType::none;
                }
            }
        } else if(&dst == &fWeapon1R || &dst == &fWeapon2R) {
            if(item.fType == eItemType::weapon) {
                const auto subtype = static_cast<eWeaponSubtype>(item.fSubType);
                if(subtype == eWeaponSubtype::bow) {
                    return false;
                } else if(subtype == eWeaponSubtype::pike) {
                    return false;
                } else {
                    if(&dst == &fWeapon1R) {
                        if(fWeapon1L.fType == eItemType::weapon) {
                            const auto subtypeL = static_cast<eWeaponSubtype>(fWeapon1L.fSubType);
                            switch(subtypeL) {
                            case eWeaponSubtype::bow:
                            case eWeaponSubtype::pike:
                                return false;
                            default:
                                return true;
                            }
                        } else {
                            return true;
                        }
                    } else { // if(&dst == &fWeapon2R) {
                        if(fWeapon2L.fType == eItemType::weapon) {
                            const auto subtypeL = static_cast<eWeaponSubtype>(fWeapon2L.fSubType);
                            switch(subtypeL) {
                            case eWeaponSubtype::bow:
                            case eWeaponSubtype::pike:
                                return false;
                            default:
                                return true;
                            }
                        } else {
                            return true;
                        }
                    }
                }
            } else if(item.fType == eItemType::shield) {
                if(&dst == &fWeapon1R) {
                    if(fWeapon1L.fType == eItemType::weapon) {
                        const auto subtypeL = static_cast<eWeaponSubtype>(fWeapon1L.fSubType);
                        switch(subtypeL) {
                        case eWeaponSubtype::bow:
                        case eWeaponSubtype::pike:
                            return false;
                        default:
                            return true;
                        }
                    } else {
                        return true;
                    }
                } else { // if(&dst == &fWeapon2R) {
                    if(fWeapon2L.fType == eItemType::weapon) {
                        const auto subtypeL = static_cast<eWeaponSubtype>(fWeapon2L.fSubType);
                        switch(subtypeL) {
                        case eWeaponSubtype::bow:
                        case eWeaponSubtype::pike:
                            return false;
                        default:
                            return true;
                        }
                    } else {
                        return true;
                    }
                }
            } else if(item.fType == eItemType::arrows) {
                if(&dst == &fWeapon1R) {
                    if(fWeapon1L.fType == eItemType::weapon) {
                        const auto subtypeL = static_cast<eWeaponSubtype>(fWeapon1L.fSubType);
                        switch(subtypeL) {
                        case eWeaponSubtype::bow:
                            return true;
                        default:
                            return false;
                        }
                    } else {
                        return true;
                    }
                } else { // if(&dst == &fWeapon2R) {
                    if(fWeapon2L.fType == eItemType::weapon) {
                        const auto subtypeL = static_cast<eWeaponSubtype>(fWeapon2L.fSubType);
                        switch(subtypeL) {
                        case eWeaponSubtype::bow:
                            return true;
                        default:
                            return false;
                        }
                    } else {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    return item.fType == type;
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

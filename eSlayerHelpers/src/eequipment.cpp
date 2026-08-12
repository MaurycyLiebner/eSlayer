#include "eSlayerHelpers/eequipment.h"

#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/evectorhelpers.h"
#include "eSlayerHelpers/eequipmentaction.h"

void eInventoryItem::read(ePacket& p) {
    fItem.read(p);
    p >> fX;
    p >> fY;
    p >> fW;
    p >> fH;
}

void eInventoryItem::write(ePacket& p) const {
    fItem.write(p);
    p << fX;
    p << fY;
    p << fW;
    p << fH;
}

uint32_t eEquipment::totalGold() const {
    return fInventoryGold + fStashGold;
}

bool eEquipment::takeGold(const uint32_t take) {
    const uint32_t total = totalGold();
    if(total < take) return false;
    const uint32_t i = std::min(take, fInventoryGold);
    fInventoryGold -= i;
    fStashGold -= take - i;
    return true;
}

bool eEquipment::replace(
    const uint32_t itemId,
    const eItem& with) {
    bool replaced = false;
    iterateOverAll([&](eItem& item) {
        if(item.fItemId != itemId) return;
        item = with;
        replaced = true;
    });
    return replaced;
}

eItem eEquipment::take(const uint32_t itemId) {
    const auto b = takeBodyItem(itemId);
    if(b.fType != eItemType::none) return b;
    for(const auto v : {&fInventory, &fBeltPotions,
                        &fBeltHiddenPotions, &fStash}) {
        const auto b = v->take(itemId);
        if(b.fType != eItemType::none) return b;
    }
    if(fTemporary.fItemId == itemId) {
        eItem result;
        std::swap(result, fTemporary);
        return result;
    }
    return eItem();
}

eItem eEquipment::item(const uint32_t itemId) const {
    const auto b = bodyItem(itemId);
    if(b.fType != eItemType::none) return b;
    for(const auto v : {&fInventory, &fBeltPotions,
                        &fBeltHiddenPotions, &fStash}) {
        const auto b = v->item(itemId);
        if(b.fType != eItemType::none) return b;
    }
    if(fTemporary.fItemId == itemId) {
        return fTemporary;
    }
    return eItem();
}

bool eEquipment::add(const eItem& item,
                     const bool reqsMet,
                     eEquipmentPlace* const pPtr) {
    if(item.fType == eItemType::potion) {
        const bool r = addToBelt(item, pPtr);
        if(r) return true;
    } else if(reqsMet) {
        bool r = tryAdd(item, ePlaceType::boots, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::gloves, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::helmet, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::armor, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::belt, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::ringL, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::ringR, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::amulet, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::weapon1L, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::weapon1R, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::weapon2L, pPtr);
        if(r) return true;
        r = tryAdd(item, ePlaceType::weapon2R, pPtr);
        if(r) return true;
    }
    uint8_t* const x = pPtr ? &pPtr->fX : nullptr;
    uint8_t* const y = pPtr ? &pPtr->fY : nullptr;
    const bool r = fInventory.tryAdd(item, x, y);
    if(r) {
        if(pPtr) {
            pPtr->fType = ePlaceType::inventory;
        }
        return true;
    }
    return false;
}

bool eEquipment::addToBelt(const eItem& item,
                           eEquipmentPlace* const pPtr) {
    if(item.fType != eItemType::potion) return false;
    const auto typeAt = [&](const int x, const int y)
        -> uint8_t {
        eInventoryItem* at = nullptr;
        if(y == fBeltVPotionSlots - 1) {
            at = fBeltPotions.at(x, 0);
        } else {
            at = fBeltHiddenPotions.at(x, y);
        }
        if(!at) return 0;
        const auto& item = at->fItem;
        const auto subtype = item.fSubType;
        return subtype;
    };

    const auto type = item.fSubType;
    for(int x = 0; x < fBeltHPotionSlots; x++) {
        const auto colType = typeAt(x, fBeltVPotionSlots - 1);
        const bool same = colType == type;
        if(same || colType == 0) {
            for(int y = fBeltVPotionSlots - 1; y >= 0; y--) {
                const auto slotType = typeAt(x, y);
                if(slotType == 0) {
                    eInventoryItem* iitem = nullptr;
                    if(y == fBeltVPotionSlots - 1) {
                        iitem = &fBeltPotions.emplace_back();
                        iitem->fY = 0;
                        if(pPtr) {
                            pPtr->fType = ePlaceType::beltPotions;
                            pPtr->fX = x;
                            pPtr->fY = 0;
                        }
                    } else {
                        iitem = &fBeltHiddenPotions.emplace_back();
                        iitem->fY = y;
                        if(pPtr) {
                            pPtr->fType = ePlaceType::beltHiddenPotions;
                            pPtr->fX = x;
                            pPtr->fY = y;
                        }
                    }
                    iitem->fItem = item;
                    iitem->fX = x;
                    iitem->fW = 1;
                    iitem->fH = 1;
                    return true;
                }
            }
        }
    }
    return false;
}

bool eBodyEquipment::canPlace(const eItem& item, const eItem& dst) {
    eItemType type = eItemType::none;
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
    } else if(&dst == &fDragged) {
        return true;
    } else if(&dst == &fTemporary) {
        return true;
    } else {
        const auto& itemData = eItemsData::get(item.fDataId);
        if(&dst == &fWeapon1L || &dst == &fWeapon2L) {
            if(item.fType != eItemType::weapon) return false;
            if(itemData.fTwoHanded) {
                const auto& otherType = &dst == &fWeapon1L ?
                    fWeapon1R.fType : fWeapon2R.fType;
                return eVectorHelpers::contains(
                    itemData.fSecondHand, otherType);
            } else {
                return true;
            }
        } else if(&dst == &fWeapon1R || &dst == &fWeapon2R) {
            if(item.fType == eItemType::weapon) {
                if(itemData.fTwoHanded) return false;
                const auto& other = &dst == &fWeapon1R ?
                    fWeapon1L : fWeapon2L;
                const int itemDataIDL = other.fDataId;
                const auto& itemDataL = eItemsData::get(itemDataIDL);
                return !itemDataL.fTwoHanded;
            } else if(item.fType == eItemType::shield) {
                const auto& other = &dst == &fWeapon1R ?
                    fWeapon1L : fWeapon2L;
                const int itemDataIDL = other.fDataId;
                const auto& itemDataL = eItemsData::get(itemDataIDL);
                return !itemDataL.fTwoHanded;
            } else if(item.fType == eItemType::arrows) {
                const auto& other = &dst == &fWeapon1R ?
                    fWeapon1L : fWeapon2L;
                const int itemDataIDL = other.fDataId;
                const auto& itemDataL = eItemsData::get(itemDataIDL);
                return eVectorHelpers::contains(
                    itemDataL.fSecondHand, eItemType::arrows);
            } else if(item.fType == eItemType::bolts) {
                const auto& other = &dst == &fWeapon1R ?
                    fWeapon1L : fWeapon2L;
                const int itemDataIDL = other.fDataId;
                const auto& itemDataL = eItemsData::get(itemDataIDL);
                return eVectorHelpers::contains(
                    itemDataL.fSecondHand, eItemType::bolts);
            }
        }
        return false;
    }
    return item.fType == type;
}

bool eBodyEquipment::tryAdd(
    const eItem& item,
    eItem& dst, const ePlaceType type,
    eEquipmentPlace* const pPtr) {
    if(dst.fType != eItemType::none) return false;
    const bool r = canPlace(item, dst);
    if(!r) return false;
    dst = item;
    if(pPtr) pPtr->fType = type;
    return true;
}

bool eBodyEquipment::tryAdd(
    const eItem& item, const ePlaceType type,
    eEquipmentPlace* const pPtr) {
    switch(type) {
    case ePlaceType::boots:
        return tryAdd(item, fBoots, type, pPtr);
    case ePlaceType::gloves:
        return tryAdd(item, fGloves, type, pPtr);
    case ePlaceType::helmet:
        return tryAdd(item, fHelmet, type, pPtr);
    case ePlaceType::armor:
        return tryAdd(item, fArmor, type, pPtr);
    case ePlaceType::belt:
        return tryAdd(item, fBelt, type, pPtr);
    case ePlaceType::ringL:
        return tryAdd(item, fRingL, type, pPtr);
    case ePlaceType::ringR:
        return tryAdd(item, fRingR, type, pPtr);
    case ePlaceType::amulet:
        return tryAdd(item, fAmulet, type, pPtr);
    case ePlaceType::weapon1L:
        return tryAdd(item, fWeapon1L, type, pPtr);
    case ePlaceType::weapon1R:
        return tryAdd(item, fWeapon1R, type, pPtr);
    case ePlaceType::weapon2L:
        return tryAdd(item, fWeapon2L, type, pPtr);
    case ePlaceType::weapon2R:
        return tryAdd(item, fWeapon2R, type, pPtr);
    case ePlaceType::dragged:
        return tryAdd(item, fDragged, type, pPtr);
    case ePlaceType::temporary:
        return tryAdd(item, fTemporary, type, pPtr);
    default:
        return false;
    }
}

void eEquipment::iterateOverAll(const eIter& iter) {
    sIterateOverAllImpl(*this, iter);
}

void eEquipment::iterateOverAll(const eCIter& iter) const {
    sIterateOverAllImpl(*this, iter);
}

eItem eEquipment::takePotion(const int x) {
    const auto at = fBeltPotions.takeAt(x, 0);
    if(at.fType != eItemType::potion) return eItem();
    for(int y = fBeltVPotionSlots - 2; y >= 0; y--) {
        const auto at = fBeltHiddenPotions.takeAt(x, y);
        if(at.fType != eItemType::potion) continue;
        eInventoryItem* iitem = nullptr;
        if(y == fBeltVPotionSlots - 2) {
            iitem = &fBeltPotions.emplace_back();
            iitem->fY = 0;
        } else {
            iitem = &fBeltHiddenPotions.emplace_back();
            iitem->fY = y + 1;
        }
        iitem->fX = x;
        iitem->fW = 1;
        iitem->fH = 1;
        iitem->fItem = at;
    }
    return at;
}

int eEquipment::beltX(const uint32_t itemId) const {
    for(const auto& it : fBeltPotions) {
        if(it.fItem.fItemId == itemId) return it.fX;
    }
    return -1;
}

void eEquipment::moveFromBody(
    eBodyEquipment& srcEq,
    std::vector<eBodyEqAction>* const moved) {
    const auto tryMove = [&](eItem eBodyEquipment::* ptr,
                             const ePlaceType place) {
        auto& src = srcEq.*ptr;
        if(src.fType == eItemType::none) return true;
        auto& dst = this->*ptr;
        if(dst.fType == eItemType::none) {
            const bool r = canPlace(src, dst);
            if(r) {
                if(moved) {
                    auto& a = moved->emplace_back();
                    a.fItemId = src.fItemId;
                    a.fPlace.fType = place;
                }
                std::swap(src, dst);
                return true;
            }
        }
        uint8_t x;
        uint8_t y;
        const bool r = fInventory.tryAdd(src, &x, &y);
        if(!r) return false;
        if(moved) {
            auto& a = moved->emplace_back();
            a.fItemId = src.fItemId;
            auto& place = a.fPlace;
            place.fType = ePlaceType::inventory;
            place.fX = x;
            place.fY = y;
        }
        src = eItem();

        return true;
    };
    tryMove(&eBodyEquipment::fBoots,
            ePlaceType::boots);
    tryMove(&eBodyEquipment::fGloves,
            ePlaceType::gloves);
    tryMove(&eBodyEquipment::fHelmet,
            ePlaceType::helmet);
    tryMove(&eBodyEquipment::fArmor,
            ePlaceType::armor);
    tryMove(&eBodyEquipment::fBelt,
            ePlaceType::belt);
    tryMove(&eBodyEquipment::fRingL,
            ePlaceType::ringL);
    tryMove(&eBodyEquipment::fRingR,
            ePlaceType::ringR);
    tryMove(&eBodyEquipment::fAmulet,
            ePlaceType::amulet);
    tryMove(&eBodyEquipment::fWeapon1L,
            ePlaceType::weapon1L);
    tryMove(&eBodyEquipment::fWeapon1R,
            ePlaceType::weapon1R);
    tryMove(&eBodyEquipment::fWeapon2L,
            ePlaceType::weapon2L);
    tryMove(&eBodyEquipment::fWeapon2R,
            ePlaceType::weapon2R);
    tryMove(&eBodyEquipment::fDragged,
            ePlaceType::dragged);
}

void eEquipment::moveFrom(eEquipment& srcEq) {
    fInventory.moveFrom(srcEq.fInventory);

    moveFromBody(static_cast<eBodyEquipment&>(srcEq));

    fBeltPotions.moveFrom(srcEq.fBeltPotions);
    fBeltHiddenPotions.moveFrom(srcEq.fBeltHiddenPotions);
    fBeltPotions.moveFrom(srcEq.fBeltHiddenPotions);
    fBeltHiddenPotions.moveFrom(srcEq.fBeltPotions);

    fInventory.moveFrom(srcEq.fBeltPotions);
    fInventory.moveFrom(srcEq.fBeltHiddenPotions);
}

bool eEquipment::empty() const {
    bool empty = true;
    iterateOverAll([&](const eItem& item) {
        if(item.fType == eItemType::none) return;
        empty = false;
    });
    return empty;
}

bool eEquipment::insertJewel(
    const uint32_t jewelId,
    const uint32_t targetId) {
    const auto jewel = item(jewelId);
    if(jewel.fType != eItemType::jewel) return false;
    auto target = item(targetId);
    if(!target.spaceForJewel()) return false;
    target.addJewel(jewel);
    take(jewelId);
    replace(targetId, target);
    return true;
}

eStrMap eBodyEquipment::partsMap() const {
    std::vector<eItemPlaceItem> items;

    const auto itemId = [](const eItem& item) -> int {
        if(item.fType == eItemType::none) return -1;
        return item.fDataId;
    };

    const auto& wL = fWeapons1 ? fWeapon1L : fWeapon2L;
    items.emplace_back(eItemPlaceItem{eItemPlace::weaponR,
                                      itemId(wL)});
    const auto& wR = fWeapons1 ? fWeapon1R : fWeapon2R;
    items.emplace_back(eItemPlaceItem{eItemPlace::weaponL,
                                      itemId(wR)});

    items.emplace_back(eItemPlaceItem{eItemPlace::helmet,
                                      itemId(fHelmet)});
    items.emplace_back(eItemPlaceItem{eItemPlace::armor,
                                      itemId(fArmor)});

    return eItemPartsMap::get(items);
}

bool eBodyEquipment::bodyEmpty() const {
    bool empty = true;
    iterateOverBody([&](eItem eBodyEquipment::*it) {
        const auto& thisV = this->*it;
        if(thisV.fType == eItemType::none) return;
        empty = false;
    });
    return empty;
}

eBodyEquipment eBodyEquipment::takeBody() {
    eBodyEquipment result;
    iterateOverBody([&](eItem eBodyEquipment::*it) {
        result.*it = this->*it;
        this->*it = eItem();
    });
    return result;
}

void eBodyEquipment::bodyRead(ePacket& p) {
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
}

void eBodyEquipment::bodyWrite(ePacket& p) const {
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
}

void eBodyEquipment::iterateOverBody(const eIter& iter) {
    iterateOverBody([this, iter](eItem eBodyEquipment::*it) {
        iter(this->*it);
    });
}

void eBodyEquipment::iterateOverBody(const eCIter& iter) const {
    iterateOverBody([this, iter](eItem eBodyEquipment::*it) {
        iter(this->*it);
    });
}

void eBodyEquipment::iterateOverBody(const eItemAction& a) {
    for(const auto it : {&eEquipment::fBoots,
                         &eEquipment::fGloves,
                         &eEquipment::fHelmet,
                         &eEquipment::fArmor,
                         &eEquipment::fBelt,
                         &eEquipment::fRingL,
                         &eEquipment::fRingR,
                         &eEquipment::fAmulet,
                         &eEquipment::fWeapon1L,
                         &eEquipment::fWeapon1R,
                         &eEquipment::fWeapon2L,
                         &eEquipment::fWeapon2R,
                         &eEquipment::fDragged}) {
        a(it);
    }
}


void eBodyEquipment::readBodyIds(ePacket& p) {
    iterateOverBody([&](eItem& item) {
        p >> item.fItemId;
    });
}

void eBodyEquipment::writeBodyIds(ePacket& p) const {
    iterateOverBody([&](const eItem& item) {
        p << item.fItemId;
    });
}

eItem eBodyEquipment::takeBodyItem(const uint32_t itemId) {
    for(const auto it : {&fBoots,
                         &fGloves,
                         &fHelmet,
                         &fArmor,
                         &fBelt,
                         &fRingL,
                         &fRingR,
                         &fAmulet,
                         &fWeapon1L,
                         &fWeapon1R,
                         &fWeapon2L,
                         &fWeapon2R,
                         &fDragged}) {
        auto& item = *it;
        if(item.fType == eItemType::none) continue;
        if(item.fItemId == itemId) {
            const auto result = item;
            item = eItem();
            return result;
        }
    }
    return eItem();
}

eItem eBodyEquipment::bodyItem(
    const uint32_t itemId) const {
    for(const auto it : {&fBoots,
                         &fGloves,
                         &fHelmet,
                         &fArmor,
                         &fBelt,
                         &fRingL,
                         &fRingR,
                         &fAmulet,
                         &fWeapon1L,
                         &fWeapon1R,
                         &fWeapon2L,
                         &fWeapon2R,
                         &fDragged}) {
        auto& item = *it;
        if(item.fType == eItemType::none) continue;
        if(item.fItemId == itemId) {
            return item;
        }
    }
    return eItem();
}

void eEquipment::read(ePacket& p) {
    bodyRead(p);
    p >> fInventoryGold;

    fInventory.read(p);
    fBeltPotions.read(p);
    fBeltHiddenPotions.read(p);
    fStash.read(p);
    p >> fStashGold;

    fTemporary.read(p);
}

void eEquipment::write(ePacket& p) const {
    bodyWrite(p);
    p << fInventoryGold;

    fInventory.write(p);
    fBeltPotions.write(p);
    fBeltHiddenPotions.write(p);
    fStash.write(p);
    p << fStashGold;

    fTemporary.write(p);
}

void eEquipment::readIds(ePacket& p) {
    iterateOverAll([&](eItem& item) {
        p >> item.fItemId;
    });
}

void eEquipment::writeIds(ePacket& p) const {
    iterateOverAll([&](const eItem& item) {
        p << item.fItemId;
    });
}

eInventoryItem* eInventoryItems::at(const int x, const int y) {
    const auto v = at(x, y, 1, 1);
    if(v.empty()) return nullptr;
    return v[0];
}

std::vector<eInventoryItem*> eInventoryItems::at(
    const int x, const int y,
    const int w, const int h) {
    std::vector<eInventoryItem*> result;
    for(auto& it : *this) {
        const bool dontOverlap = x >= it.fX + it.fW ||
                                 x + w <= it.fX ||
                                 y >= it.fY + it.fH ||
                                 y + h <= it.fY;
        if(dontOverlap) continue;
        result.emplace_back(&it);
        if(w == 1 && h == 1) return result;
    }
    return result;
}

eItem eInventoryItems::takeAt(const int x, const int y) {
    const int w = 1;
    const int h = 1;
    for(int i = 0; i < size(); i++) {
        const auto& it = (*this)[i];
        const bool dontOverlap = x >= it.fX + it.fW ||
                                 x + w <= it.fX ||
                                 y >= it.fY + it.fH ||
                                 y + h <= it.fY;
        if(dontOverlap) continue;
        const auto result = it.fItem;
        erase(begin() + i);
        return result;
    }
    return eItem();
}

void eInventoryItems::moveFrom(eInventoryItems& src) {
    for(int i = 0; i < src.size(); i++) {
        const auto& it = src[i];
        const auto v = at(it.fX, it.fY, it.fW, it.fH);
        if(!v.empty()) continue;
        emplace_back(it);
        src.erase(src.begin() + i);
        i--;
    }
    for(int i = 0; i < src.size(); i++) {
        const auto& it = src[i];
        const bool r = tryAdd(it.fItem);
        if(r) {
            src.erase(src.begin() + i);
            i--;
        }
    }
}

bool eInventoryItems::tryAdd(
    const eItem& item,
    uint8_t* const xPtr,
    uint8_t* const yPtr) {
    if(item.fType == eItemType::none) return true;
    const auto& itemData = eItemsData::get(item.fDataId);
    const int w = itemData.fWidth;
    const int h = itemData.fHeight;
    for(int x = 0; x <= mWidth - w; x++) {
        for(int y = 0; y <= mHeight - h; y++) {
            const auto v = at(x, y, w, h);
            const bool overlap = !v.empty();
            if(!overlap) {
                auto& iitem = emplace_back();
                if(xPtr) *xPtr = x;
                if(yPtr) *yPtr = y;
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

bool eInventoryItems::tryAdd(
    const eItem& item,
    const uint8_t x,
    const uint8_t y) {
    if(item.fType == eItemType::none) return true;
    const auto& itemData = eItemsData::get(item.fDataId);
    const int w = itemData.fWidth;
    const int h = itemData.fHeight;
    if(w <= 0 || h <= 0) return false;
    const auto v = at(x, y, w, h);
    const bool overlap = !v.empty();
    if(overlap) return false;
    auto& iitem = emplace_back();
    iitem.fItem = item;
    iitem.fX = x;
    iitem.fY = y;
    iitem.fW = w;
    iitem.fH = h;
    return true;
}

eItem eInventoryItems::take(const uint32_t itemId) {
    for(int i = 0; i < size(); i++) {
        const auto& it = (*this)[i];
        const auto& item = it.fItem;
        if(item.fType == eItemType::none) continue;
        if(item.fItemId == itemId) {
            const auto result = item;
            erase(begin() + i);
            return result;
        }
    }
    return eItem();
}

eItem eInventoryItems::item(const uint32_t itemId) const {
    for(int i = 0; i < size(); i++) {
        const auto& it = (*this)[i];
        const auto& item = it.fItem;
        if(item.fType == eItemType::none) continue;
        if(item.fItemId == itemId) {
            return item;
        }
    }
    return eItem();
}

bool eInventoryItems::setItemId(
    const uint32_t itemId,
    const uint32_t newItemId) {
    for(int i = 0; i < size(); i++) {
        auto& it = (*this)[i];
        auto& item = it.fItem;
        if(item.fType == eItemType::none) continue;
        if(item.fItemId == itemId) {
            item.fItemId = newItemId;
            return true;
        }
    }
    return false;
}

void eInventoryItems::read(ePacket& p) {
    uint16_t nitems;
    p >> nitems;
    for(int i = 0; i < nitems; i++) {
        auto& item = emplace_back();
        item.read(p);
    }
}

void eInventoryItems::write(ePacket& p) const {
    const uint16_t nitems = size();
    p << nitems;
    for(const auto& item : *this) {
        item.write(p);
    }
}

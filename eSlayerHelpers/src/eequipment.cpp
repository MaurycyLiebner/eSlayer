#include "eSlayerHelpers/eequipment.h"

#include "eSlayerHelpers/eitemsdata.h"
#include "eSlayerHelpers/epacket.h"
#include "eSlayerHelpers/epotiontype.h"
#include "eSlayerHelpers/evectorhelpers.h"

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

eItem eEquipment::get(const uint32_t itemId) const {
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
        if(item.fItemId == itemId) return item;
    }
    for(const auto v : {&fInventory, &fBeltPotions, &fBeltHiddenPotions, &fStash}) {
        for(const auto& it : *v) {
            const auto& item = it.fItem;
            if(item.fType == eItemType::none) continue;
            if(item.fItemId == itemId) return item;
        }
    }
    return eItem();
}

eItem eEquipment::take(const uint32_t itemId) {
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
    for(const auto v : {&fInventory, &fBeltPotions, &fBeltHiddenPotions, &fStash}) {
        for(int i = 0; i < v->size(); i++) {
            const auto& it = (*v)[i];
            const auto& item = it.fItem;
            if(item.fType == eItemType::none) continue;
            if(item.fItemId == itemId) {
                const auto result = item;
                v->erase(v->begin() + i);
                return result;
            }
        }
    }
    return eItem();
}

bool eEquipment::add(const eItem& item, const bool reqsMet) {
    if(item.fType == eItemType::potion) {
        const bool r = addToBelt(item);
        if(r) return true;
    } else if(reqsMet) {
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
    }
    return fInventory.tryAdd(item);
}

bool eEquipment::addToBelt(const eItem& item) {
    if(item.fType != eItemType::potion) return false;
    const auto typeAt = [&](const int x, const int y) {
        eInventoryItem* at = nullptr;
        if(y == fBeltVPotionSlots - 1) {
            at = fBeltPotions.at(x, 0);
        } else {
            at = fBeltHiddenPotions.at(x, y);
        }
        if(!at) return ePotionType::none;
        const auto& item = at->fItem;
        const auto subtype = item.fSubType;
        const auto pType = static_cast<ePotionType>(subtype);
        return pType;
    };

    const auto type = static_cast<ePotionType>(item.fSubType);
    for(int x = 0; x < fBeltHPotionSlots; x++) {
        const auto colType = typeAt(x, fBeltVPotionSlots - 1);
        const bool same = ePotionTypeHelpers::sameCategory(colType, type);
        if(same || colType == ePotionType::none) {
            for(int y = fBeltVPotionSlots - 1; y >= 0; y--) {
                const auto slotType = typeAt(x, y);
                if(slotType == ePotionType::none) {
                    eInventoryItem* iitem = nullptr;
                    if(y == fBeltVPotionSlots - 1) {
                        iitem = &fBeltPotions.emplace_back();
                        iitem->fY = 0;
                    } else {
                        iitem = &fBeltHiddenPotions.emplace_back();
                        iitem->fY = y;
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

bool eEquipment::canPlace(const eItem& item, const eItem& dst) {
    eItemType type = eItemType::none;
    const auto& itemData = eItemsData::get(item.fDataId);
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

void eEquipment::moveFromBody(eBodyEquipment& srcEq) {
    const auto tryMove = [&](eItem eBodyEquipment::* ptr) {
        auto& src = srcEq.*ptr;
        if(src.fType == eItemType::none) return true;
        auto& dst = this->*ptr;
        if(dst.fType == eItemType::none) {
            const bool r = canPlace(src, dst);
            if(r) {
                std::swap(src, dst);
                return true;
            }
        }
        const bool r = fInventory.tryAdd(src);
        if(!r) return false;
        src = eItem();

        return true;
    };
    tryMove(&eBodyEquipment::fBoots);
    tryMove(&eBodyEquipment::fGloves);
    tryMove(&eBodyEquipment::fHelmet);
    tryMove(&eBodyEquipment::fArmor);
    tryMove(&eBodyEquipment::fBelt);
    tryMove(&eBodyEquipment::fRingL);
    tryMove(&eBodyEquipment::fRingR);
    tryMove(&eBodyEquipment::fAmulet);
    tryMove(&eBodyEquipment::fWeapon1L);
    tryMove(&eBodyEquipment::fWeapon1R);
    tryMove(&eBodyEquipment::fWeapon2L);
    tryMove(&eBodyEquipment::fWeapon2R);
    tryMove(&eBodyEquipment::fDragged);
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

void eEquipment::read(ePacket& p) {
    bodyRead(p);
    p >> fInventoryGold;

    fInventory.read(p);
    fBeltPotions.read(p);
    fBeltHiddenPotions.read(p);
    fStash.read(p);
    p >> fStashGold;
}

void eEquipment::write(ePacket& p) const {
    bodyWrite(p);
    p << fInventoryGold;

    fInventory.write(p);
    fBeltPotions.write(p);
    fBeltHiddenPotions.write(p);
    fStash.write(p);
    p << fStashGold;
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

bool eInventoryItems::tryAdd(const eItem& item) {
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

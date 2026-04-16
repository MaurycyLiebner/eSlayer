#ifndef EEQUIPMENT_H
#define EEQUIPMENT_H

#include "eslayerhelpersexport.h"

#include "eitem.h"

struct ESLAYERHELPERS_API eInventoryItem {
    eItem fItem;
    int fX;
    int fY;
    int fW;
    int fH;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

struct ESLAYERHELPERS_API eInventoryItems :
    public std::vector<eInventoryItem> {
    eInventoryItem* at(const int x, const int y);
    eItem takeAt(const int x, const int y);

    void read(ePacket& p);
    void write(ePacket& p) const;
};

struct ESLAYERHELPERS_API eEquipment {
    eItem fBoots;
    eItem fGloves;
    eItem fHelmet;
    eItem fArmor;
    eItem fBelt;
    eItem fRingL;
    eItem fRingR;
    eItem fAmulet;
    eItem fWeapon1L;
    eItem fWeapon1R;
    eItem fWeapon2L;
    eItem fWeapon2R;
    bool fWeapons1 = true;

    eItem fDragged;

    static const int fBeltHPotionSlots = 4;
    static const int fBeltVPotionSlots = 4;
    eInventoryItems fBeltHiddenPotions;
    eInventoryItems fBeltPotions;

    static const int fInventoryWidth = 10;
    static const int fInventoryHeight = 4;
    eInventoryItems fInventory;

    static const int fStashWidth = 10;
    static const int fStashHeight = 8;
    eInventoryItems fStash;

    eItem get(const uint32_t itemId) const;
    eItem take(const uint32_t itemId);
    bool add(const eItem& item);
    bool addToBelt(const eItem& item);
    bool canPlace(const eItem& item, const eItem& dst);

    using eIter = std::function<void(eItem& item)>;
    void iterateOverAll(const eIter& iter);

    eItem takePotion(const int x);
    int beltX(const uint32_t itemId) const;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EEQUIPMENT_H

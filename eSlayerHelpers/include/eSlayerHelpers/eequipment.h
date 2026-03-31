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

    static const int fInventoryWidth = 10;
    static const int fInventoryHeight = 4;
    std::vector<eInventoryItem> fInventory;

    eItem get(const int itemId) const;
    bool add(const eItem& item);
    bool canPlace(const eItem& item, const eItem& dst);

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EEQUIPMENT_H

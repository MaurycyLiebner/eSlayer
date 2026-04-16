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

    static const int fBeltHPotionSlots = 4;
    static const int fBeltVPotionSlots = 4;
    std::vector<eInventoryItem> fBeltHiddenPotions;
    std::vector<eInventoryItem> fBeltPotions;

    static const int fInventoryWidth = 10;
    static const int fInventoryHeight = 4;
    std::vector<eInventoryItem> fInventory;

    static const int fStashWidth = 10;
    static const int fStashHeight = 8;
    std::vector<eInventoryItem> fStash;

    eItem get(const int itemId) const;
    bool add(const eItem& item);
    bool canPlace(const eItem& item, const eItem& dst);

    using eIter = std::function<void(eItem& item)>;
    void iterateOverAll(const eIter& iter);

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EEQUIPMENT_H

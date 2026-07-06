#ifndef EEQUIPMENT_H
#define EEQUIPMENT_H

#include "eslayerhelpersexport.h"

#include "eitem.h"
#include "eequipmentplace.h"

struct eBodyEqAction;

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
    eInventoryItems() {}
    eInventoryItems(const int width, const int height) :
        mWidth(width), mHeight(height) {}

    eInventoryItem* at(const int x, const int y);
    std::vector<eInventoryItem*> at(
        const int x, const int y,
        const int w, const int h);
    eItem takeAt(const int x, const int y);
    void moveFrom(eInventoryItems& src);
    bool tryAdd(const eItem& item,
                uint8_t* const xPtr = nullptr,
                uint8_t* const yPtr = nullptr);
    bool tryAdd(const eItem& item,
                const uint8_t x,
                const uint8_t y);
    eItem take(const uint32_t itemId);
    eItem item(const uint32_t itemId) const;
    bool setItemId(const uint32_t itemId,
                   const uint32_t newItemId);

    void read(ePacket& p);
    void write(ePacket& p) const;
private:
    int mWidth;
    int mHeight;
};

struct ESLAYERHELPERS_API eBodyEquipment {
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

    bool bodyEmpty() const;
    eBodyEquipment takeBody();

    void bodyRead(ePacket& p);
    void bodyWrite(ePacket& p) const;

    void readBodyIds(ePacket& p);
    void writeBodyIds(ePacket& p) const;

    eItem takeBodyItem(const uint32_t itemId);
    eItem bodyItem(const uint32_t itemId) const;

    using eIter = std::function<void(eItem& item)>;
    void iterateOverBody(const eIter& iter);
    using eCIter = std::function<void(const eItem& item)>;
    void iterateOverBody(const eCIter& iter) const;
private:
    using eItemAction = std::function<void(eItem eBodyEquipment::*it)>;
    static void iterateOverBody(const eItemAction& a);
};

struct ESLAYERHELPERS_API eEquipment : public eBodyEquipment  {
    static const int fBeltHPotionSlots = 4;
    static const int fBeltVPotionSlots = 4;
    eInventoryItems fBeltHiddenPotions{fBeltHPotionSlots,
                                       fBeltVPotionSlots - 1};
    eInventoryItems fBeltPotions{fBeltHPotionSlots, 1};

    static const int fInventoryWidth = 10;
    static const int fInventoryHeight = 4;
    eInventoryItems fInventory{fInventoryWidth,
                               fInventoryHeight};
    uint32_t fInventoryGold = 0;

    static const int fStashWidth = 10;
    static const int fStashHeight = 8;
    eInventoryItems fStash{fStashWidth,
                           fStashHeight};
    uint32_t fStashGold = 0;

    uint32_t totalGold() const;
    bool takeGold(const uint32_t take);

    bool replace(const uint32_t itemId, const eItem& with);
    eItem take(const uint32_t itemId);
    eItem item(const uint32_t itemId) const;
    bool add(const eItem& item, const bool reqsMet,
             eEquipmentPlace* const pPtr);
    bool addToBelt(const eItem& item, eEquipmentPlace* const pPtr);
    bool canPlace(const eItem& item, const eItem& dst);

    using eIter = std::function<void(eItem& item)>;
    void iterateOverAll(const eIter& iter);
    using eCIter = std::function<void(const eItem& item)>;
    void iterateOverAll(const eCIter& iter) const;

    eItem takePotion(const int x);
    int beltX(const uint32_t itemId) const;

    void moveFromBody(eBodyEquipment& srcEq,
                      std::vector<eBodyEqAction>* const moved = nullptr);
    void moveFrom(eEquipment& srcEq);
    bool empty() const;

    bool insertJewel(const uint32_t jewelId,
                     const uint32_t targetId);

    void read(ePacket& p);
    void write(ePacket& p) const;

    void readIds(ePacket& p);
    void writeIds(ePacket& p) const;

    template <typename Self, typename Iter>
    static void sIterateOverAllImpl(Self& self, Iter&& iter) {
        iter(self.fBoots);
        iter(self.fGloves);
        iter(self.fHelmet);
        iter(self.fArmor);
        iter(self.fBelt);
        iter(self.fRingL);
        iter(self.fRingR);
        iter(self.fAmulet);
        iter(self.fWeapon1L);
        iter(self.fWeapon1R);
        iter(self.fWeapon2L);
        iter(self.fWeapon2R);
        iter(self.fDragged);

        for(auto v : {&self.fInventory, &self.fBeltPotions, &self.fBeltHiddenPotions, &self.fStash}) {
            for(auto& item : *v) {
                iter(item.fItem);
            }
        }
    }
};

#endif // EEQUIPMENT_H

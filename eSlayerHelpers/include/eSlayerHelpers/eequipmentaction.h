#ifndef EEQUIPMENTACTION_H
#define EEQUIPMENTACTION_H

#include "eslayerhelpersexport.h"

#include "eitem.h"
#include "eequipmentplace.h"

struct eEquipment;

enum class eEquipmentActionType {
    none, add,
    drag, switchDrag, drop,
    gold, switchWeapons,
    dragAndDrop
};

struct eBodyEqAction {
    eEquipmentPlace fPlace;
    uint32_t fItemId;
};

struct ESLAYERHELPERS_API eBuyAction {
    eEquipmentPlace fPlace;

    uint32_t fSellerId;
    uint32_t fItemId;
};

struct ESLAYERHELPERS_API eEquipmentAction {
    eEquipmentActionType fType = eEquipmentActionType::none;
    eEquipmentPlace fPlace;

    uint32_t fSellerId;

    eItem fAddItem;

    uint32_t fItemId1;
    uint32_t fItemId2;

    uint32_t fInvGold;
    uint32_t fStashGold;

    bool fWeapons1;

    bool apply(eEquipment& eq) const;

    void read(ePacket& p);
    void write(ePacket& p) const;

    static bool add(eEquipment& eq,
                    const eItem& item,
                    const eEquipmentPlace place);
private:
    bool add(eEquipment& eq) const;
    bool drag(eEquipment& eq) const;
    bool switchDrag(eEquipment& eq) const;
    bool drop(eEquipment& eq) const;
    bool dragAndDrop(eEquipment& eq) const;
    bool gold(eEquipment& eq) const;
    bool switchWeapons(eEquipment& eq) const;
};

#endif // EEQUIPMENTACTION_H

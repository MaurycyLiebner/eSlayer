#ifndef EEQUIPMENTACTION_H
#define EEQUIPMENTACTION_H

#include "eitem.h"
#include "eequipmentplace.h"

struct eEquipment;

enum class eEquipmentActionType {
    none, add
};

struct eEquipmentAction {
    eEquipmentActionType fType = eEquipmentActionType::none;
    eEquipmentPlace fPlace;
    eItem fItem;

    bool apply(eEquipment& eq) const;

    void read(ePacket& p);
    void write(ePacket& p) const;
private:
    bool add(eEquipment& eq) const;
};

#endif // EEQUIPMENTACTION_H

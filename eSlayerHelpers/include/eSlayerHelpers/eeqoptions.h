#ifndef EEQOPTIONS_H
#define EEQOPTIONS_H

#include "eitemdata.h"
#include "eequipmentplace.h"

struct eItem;

struct ESLAYERHELPERS_API eEqOptions {
    std::vector<ePlaceType> fEquipment;
    std::vector<eItemType> fTypes;
    std::vector<uint8_t> fWeaponClasses;

    bool validateItem(const eItem& item) const;
};

#endif // EEQOPTIONS_H

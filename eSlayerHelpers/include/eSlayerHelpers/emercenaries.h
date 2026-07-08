#ifndef EMERCENARIES_H
#define EMERCENARIES_H

#include "eequipmentplace.h"

#include "estringidmapvector.h"

#include <cstdint>
#include <vector>

struct eMercenaryInfo {
    uint8_t fUnitType;

    std::vector<ePlaceType> fEquipment;
    std::vector<uint8_t> fWeaponClasses;
};

class ESLAYERHELPERS_API eMercenariesInfo {
public:
    static eStringIdMapVector<eMercenaryInfo> sMercs;

    static void load();
private:
    static bool sLoaded;
};

#endif // EMERCENARIES_H

#ifndef EREQUESTDATA_H
#define EREQUESTDATA_H

#include "eunitdata.h"
#include "eunitdynamicdata.h"
#include "emissile.h"
#include "egrounditem.h"

class ePacket;

struct ESLAYERHELPERS_API eRequestData {
    uint32_t fRequestId;
    std::vector<eUnitData> fNewUnits;
    std::vector<eUnitDynamicData> fUpdatedUnits;
    std::vector<eMissile> fMissiles;
    std::vector<eGroundItem> fNewItems;
    std::vector<uint32_t> fRemovedItemIds;
    uint16_t fMana;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EREQUESTDATA_H

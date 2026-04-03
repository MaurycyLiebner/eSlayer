#ifndef EREQUESTDATA_H
#define EREQUESTDATA_H

#include "eunitdata.h"
#include "eunitdynamicdata.h"
#include "emissile.h"
#include "egrounditem.h"
#include "emapportion.h"

class ePacket;

struct ESLAYERHELPERS_API eRequestData {
    uint32_t fRequestId;
    std::vector<eUnitData> fNewUnits;
    std::vector<eUnitDynamicData> fUpdatedUnits;
    std::vector<eMissile> fMissiles;
    std::vector<eGroundItem> fNewItems;
    std::vector<uint32_t> fRemovedItemIds;
    bool fHasMap = false;
    eMapPortion fMapPortion;
    uint16_t fMana;
    uint16_t fLevel;
    uint16_t fExperience;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EREQUESTDATA_H

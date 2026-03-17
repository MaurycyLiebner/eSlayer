#ifndef EREQUESTDATA_H
#define EREQUESTDATA_H

#include "eunitdata.h"
#include "emissile.h"

#include "epacket.h"

struct eRequestData {
    uint32_t fRequestId;
    std::vector<eUnitData> fUnits;
    std::vector<eMissile> fMissiles;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EREQUESTDATA_H

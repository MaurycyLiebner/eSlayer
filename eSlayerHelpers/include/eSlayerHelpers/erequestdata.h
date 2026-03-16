#ifndef EREQUESTDATA_H
#define EREQUESTDATA_H

#include "eunitdata.h"

#include "epacket.h"

struct eRequestData {
    uint32_t fRequestId;
    std::vector<eUnitData> fUnits;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EREQUESTDATA_H

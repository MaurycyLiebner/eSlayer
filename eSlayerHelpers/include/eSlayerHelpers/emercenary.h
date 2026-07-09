#ifndef EMERCENARY_H
#define EMERCENARY_H

#include <cstdint>

#include "eSlayerHelpers/eequipment.h"

struct ESLAYERHELPERS_API eMercenary {
    uint32_t fUnitId = 0; // 0 when dead
    uint8_t fMercType = 0;
    uint8_t fNameId = 0;
    uint8_t fLevel = 1;
    float fExp = 0.f;
    eEquipment fEq;
    bool fDead = true;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EMERCENARY_H

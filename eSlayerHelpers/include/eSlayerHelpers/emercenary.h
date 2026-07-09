#ifndef EMERCENARY_H
#define EMERCENARY_H

#include <cstdint>

#include "eSlayerHelpers/eequipment.h"
#include "eSlayerHelpers/eattributes.h"

struct ESLAYERHELPERS_API eMercenaryBase {
    uint8_t fMercType = 0;
    uint8_t fNameId = 0;
    uint8_t fLevel = 1;

    eAttributes attributes() const;
    std::vector<eModifier> mods() const;
    uint32_t cost() const;
};

struct ESLAYERHELPERS_API eMercenary :
    public eMercenaryBase {
    uint32_t fUnitId = 0; // 0 when dead
    float fExp = 0.f;
    eEquipment fEq;
    bool fDead = true;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EMERCENARY_H

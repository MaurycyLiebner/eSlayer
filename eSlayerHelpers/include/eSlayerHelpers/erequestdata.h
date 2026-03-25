#ifndef EREQUESTDATA_H
#define EREQUESTDATA_H

#include "eunitdata.h"
#include "emissile.h"

#include "epacket.h"

struct ESLAYERHELPERS_API eEquipmentData {
    eWeaponType fWeaponType = eWeaponType::ranged;
    float fMeeleRange = 0.f;
    float fRangedRange = 4.f;
    int fMissileId = -1;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

struct ESLAYERHELPERS_API eRequestData {
    uint32_t fRequestId;
    std::vector<eUnitData> fUnits;
    std::vector<eMissile> fMissiles;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EREQUESTDATA_H

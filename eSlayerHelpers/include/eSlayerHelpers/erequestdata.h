#ifndef EREQUESTDATA_H
#define EREQUESTDATA_H

#include "eunitdata.h"
#include "eunitdynamicdata.h"
#include "emissile.h"
#include "eweapontype.h"
#include "egrounditem.h"

class ePacket;

struct ESLAYERHELPERS_API eWeaponData {
    eWeaponType fWeaponTypeL = eWeaponType::meele;
    eWeaponType fWeaponTypeR = eWeaponType::meele;
    float fMeeleRange = 0.f;
    float fRangedRange = 0.f;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

struct ESLAYERHELPERS_API eRequestData {
    uint32_t fRequestId;
    std::vector<eUnitData> fNewUnits;
    std::vector<eUnitDynamicData> fUpdatedUnits;
    std::vector<eMissile> fMissiles;
    std::vector<eGroundItem> fNewItems;
    std::vector<uint32_t> fRemovedItemIds;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EREQUESTDATA_H

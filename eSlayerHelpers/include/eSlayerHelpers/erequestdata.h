#ifndef EREQUESTDATA_H
#define EREQUESTDATA_H

#include "eunitdata.h"
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
    std::vector<eUnitData> fUnits;
    std::vector<eMissile> fMissiles;
    std::vector<eGroundItem> fItems;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EREQUESTDATA_H

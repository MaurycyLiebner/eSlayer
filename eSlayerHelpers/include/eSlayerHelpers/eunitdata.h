#ifndef EUNITDATA_H
#define EUNITDATA_H

#include "eslayerhelpersexport.h"

#include "echardata.h"
#include "eunitdynamicdata.h"
#include "eteamid.h"

class ePacket;

const uint8_t sFleshExplAnim = 255;
const uint8_t sIceExplAnim = 254;

struct ESLAYERHELPERS_API eUnitData :
    public eUnitDynamicData {
    eTeamId fTeamId;

    uint8_t fUnitInfoId;

    float fRadius;

    std::set<uint8_t> fMods;

    eModelParts fModelParts;

    eUnitData toUnitData() const;
    eUnitDynamicData toDynamicData(const uint8_t update) const;

    void read(ePacket& p);
    void write(ePacket& p) const;

    static float sColdSpeed;
};

#endif // EUNITDATA_H

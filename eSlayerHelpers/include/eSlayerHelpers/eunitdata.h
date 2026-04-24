#ifndef EUNITDATA_H
#define EUNITDATA_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"
#include "echardata.h"
#include "eunitdynamicdata.h"
#include "eteamid.h"

class ePacket;

struct ESLAYERHELPERS_API eUnitData : public ePositioned {
    int32_t fCharId;
    eTeamId fTeamId;

    uint8_t fCharDataId;

    float fRadius;

    float fAngle;

    uint8_t fAnim;
    uint32_t fAnimId;
    float fAnimSpeed;

    float fBlockingActionTime;

    uint16_t fHealth;
    uint16_t fMaxHealth;

    eModelParts fModelParts;

    eUnitData toUnitData() const;
    eUnitDynamicData toDynamicData() const;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EUNITDATA_H

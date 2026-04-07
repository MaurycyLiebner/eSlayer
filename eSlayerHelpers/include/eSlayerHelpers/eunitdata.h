#ifndef EUNITDATA_H
#define EUNITDATA_H

#include "eslayerhelpersexport.h"

#include "epacket.h"

#include "epositioned.h"
#include "evec2.h"
#include "echardata.h"
#include "eunitdynamicdata.h"

struct ESLAYERHELPERS_API eUnitData : public ePositioned {
    int32_t fCharId;
    int8_t fTeamId;

    uint8_t fCharDataId;

    float fRadius;

    eVec2f fVel;
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

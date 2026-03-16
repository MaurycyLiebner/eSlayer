#ifndef EUNITDATA_H
#define EUNITDATA_H

#include "eslayerhelpersexport.h"

#include "epacket.h"

#include "epoint.h"
#include "evec2.h"
#include "echardata.h"

struct ESLAYERHELPERS_API eUnitData {
    int32_t fCharId;
    int8_t fTeamId;

    uint8_t fTypeId;

    float fRadius;

    ePointF fPos;
    eVec2f fVel;
    float fAngle;

    uint8_t fAnim;
    uint32_t fAnimId;
    float fAnimSpeed;

    float fActionTime;

    uint16_t fHealth;
    uint16_t fMaxHealth;

    eCompressedModelParts fModelParts;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EUNITDATA_H

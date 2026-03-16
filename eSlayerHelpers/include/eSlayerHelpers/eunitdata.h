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

    double fRadius;

    ePointF fPos;
    eVec2d fVel;
    double fAngle;

    uint8_t fAnim;
    uint32_t fAnimId;
    double fAnimSpeed;

    double fActionTime;

    uint16_t fHealth;
    uint16_t fMaxHealth;

    eCompressedModelParts fModelParts;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EUNITDATA_H

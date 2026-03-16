#ifndef EUNITDATA_H
#define EUNITDATA_H

#include "eslayerhelpersexport.h"

#include "epacket.h"

#include "epoint.h"
#include "evec2.h"
#include "echardata.h"

struct ESLAYERHELPERS_API eUnitData {
    int32_t fCharId;
    int32_t fTeamId;

    int32_t fTypeId;

    double fRadius;

    ePointF fPos;
    eVec2d fVel;
    double fAngle;

    int32_t fAnim;
    int32_t fAnimId;
    double fAnimSpeed;

    double fActionTime;

    int32_t fHealth;
    int32_t fMaxHealth;

    eCompressedModelParts fModelParts;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EUNITDATA_H

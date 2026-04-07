#ifndef EUNITDYNAMICDATA_H
#define EUNITDYNAMICDATA_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"
#include "evec2.h"

class ePacket;

struct ESLAYERHELPERS_API eUnitDynamicData : public ePositioned {
    int32_t fCharId;

    eVec2f fVel;
    float fAngle;

    uint8_t fAnim;
    uint32_t fAnimId;
    float fAnimSpeed;

    float fBlockingActionTime;

    uint16_t fHealth;
    uint16_t fMaxHealth;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EUNITDYNAMICDATA_H

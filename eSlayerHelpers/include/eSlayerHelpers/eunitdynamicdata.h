#ifndef EUNITDYNAMICDATA_H
#define EUNITDYNAMICDATA_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"

#include <set>

class ePacket;

struct ESLAYERHELPERS_API eUnitDynamicData : public ePositioned {
    uint32_t fCharId;

    float fAngle;

    uint8_t fAnim;
    uint32_t fAnimId;
    float fAnimSpeed;

    float fBlockingActionTime;

    uint16_t fHealth;
    uint16_t fMaxHealth;

    uint8_t fState;

    std::set<uint8_t> fBoosts;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EUNITDYNAMICDATA_H

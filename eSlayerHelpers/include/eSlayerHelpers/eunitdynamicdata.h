#ifndef EUNITDYNAMICDATA_H
#define EUNITDYNAMICDATA_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"

#include <eSlayerHelpers/eanimid.h>

#include <set>

class ePacket;

struct ESLAYERHELPERS_API eUnitDynamicDataBase {
    uint32_t fCharId;

    uint8_t fAnim;
    eAnimId fAnimId;
    float fAnimSpeed;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

struct ESLAYERHELPERS_API eUnitDynamicData :
    public eUnitDynamicDataBase,
    public ePositioned {
    float fAngle;

    float fBlockingActionTime;

    uint16_t fHealth;
    uint16_t fMaxHealth;

    uint8_t fState;

    std::set<uint8_t> fBoosts;

    void read(ePacket& p);
    void write(ePacket& p) const;
private:
    using eUnitDynamicDataBase::write;
    using eUnitDynamicDataBase::read;
};

struct ESLAYERHELPERS_API eDeadUnitDynamicData :
    public eUnitDynamicDataBase {
};

#endif // EUNITDYNAMICDATA_H

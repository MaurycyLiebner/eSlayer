#ifndef EUNITSPECIALDATA_H
#define EUNITSPECIALDATA_H

#include "epoint.h"

#include <cstdint>

struct eUnitData;

struct ESLAYERHELPERS_API eUnitSpecialData {
    uint32_t fClientId;
    uint8_t fUnitType;
    ePointF fPos;
    uint8_t fMapId = 0;
    uint8_t fAreaId = 0;
    uint16_t fHealth = 100;
    uint16_t fMaxHealth = 100;

    void assign(const eUnitData& data);
};

#endif // EUNITSPECIALDATA_H

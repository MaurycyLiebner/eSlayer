#ifndef EMISSILECOLLISION_H
#define EMISSILECOLLISION_H

#include "eslayermissilesexport.h"

#include <eSlayerHelpers/epoint.h>

#include <cmath>

struct eUnitData;
struct eMissile;

struct ESLAYERMISSILES_API eMissileCollision {
    struct eResult {
        bool fHit = false;
        float fT = 2.f;
        uint32_t fCharId = 0;
    };

    static void test(const ePointF& oldPos,
                     const ePointF& newPos,
                     const ePointF& unitPos,
                     const float collR,
                     const uint32_t charId,
                     eResult& result);

    static void test(const ePointF& oldPos,
                     const ePointF& newPos,
                     const eUnitData& u,
                     const eMissile& m,
                     eResult& result);
};

#endif // EMISSILECOLLISION_H

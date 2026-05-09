#ifndef ENOVA_H
#define ENOVA_H

#include "eslayerhelpersexport.h"

#include "eteamid.h"
#include "epoint.h"

struct eArcInterval {
    float fAngleStart;
    float fAngleEnd;
};

struct ESLAYERHELPERS_API eNova {
    uint32_t fId;
    eTeamId fTeamId;
    uint8_t fMissileType;

    ePointF fCenter;

    float fRadius;
    float fMaxRadius;
    float fSpeed;

    std::vector<eArcInterval> fIntervals{{0.f, 360.f}};

    void obsticle1(const ePointF& pos,
                   const float radius);
    void obsticle4(const ePointF& pos1,
                   const ePointF& pos2,
                   const ePointF& pos3,
                   const ePointF& pos4);
    void subtract(float minAngleDeg,
                  float maxAngleDeg);

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // ENOVA_H

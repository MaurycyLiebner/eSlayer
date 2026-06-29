#ifndef ENOVA_H
#define ENOVA_H

#include "eslayerhelpersexport.h"

#include "eteamid.h"
#include "epoint.h"
#include "earcintervals.h"

struct ESLAYERHELPERS_API eNova {
    uint32_t fId;
    eTeamId fTeamId;
    uint8_t fMissileType;

    ePointF fCenter;

    float fRadius;
    float fMaxRadius;
    float fSpeed;

    eArcIntervals fIntervals{{0.f, 360.f}};

    void obstacle1(const ePointF& pos,
                   const float radius);
    void obstacle2(const ePointF& pos1,
                   const ePointF& pos2);
    void obstacle4(const ePointF& pos,
                   const float width,
                   const float height);
    void obstacle4(const ePointF& pos1,
                   const ePointF& pos2,
                   const ePointF& pos3,
                   const ePointF& pos4);
    bool angleInRange(const float angle) const;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // ENOVA_H

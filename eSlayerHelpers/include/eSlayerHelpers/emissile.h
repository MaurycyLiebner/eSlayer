#ifndef EMISSILE_H
#define EMISSILE_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"

class ePacket;

struct ESLAYERHELPERS_API eMissile : public ePositioned {
    int32_t fId;
    int8_t fTeamId;
    uint8_t fType;
    uint8_t fPathType;
    ePointF fFrom;
    ePointF fTo;
    float fSpeed;
    // number of obsticles missile can hit (0 - infinity)
    uint8_t fPierced;
    float fRemDist;
    float fRadius;
    float fTime;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

struct eExtendedMissile : public eMissile {
    float fAngle;
};

#endif // EMISSILE_H

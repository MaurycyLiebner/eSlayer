#ifndef EMISSILE_H
#define EMISSILE_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"

class ePacket;

struct ESLAYERHELPERS_API eMissile : public ePositioned {
    int32_t fId;
    uint8_t fType;
    uint8_t fPathType;
    ePointF fFrom;
    ePointF fTo;
    float fSpeed;
    // number of obsticles missile can hit (0 - infinity)
    uint8_t fObsticles;
    float fRemDist;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EMISSILE_H

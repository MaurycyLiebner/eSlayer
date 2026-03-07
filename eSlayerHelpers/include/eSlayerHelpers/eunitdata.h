#ifndef EUNITDATA_H
#define EUNITDATA_H

#include "eslayerhelpersexport.h"

#include "epoint.h"
#include "evec2.h"

struct ESLAYERHELPERS_API eUnitData {
    int fCharId;
    int fTeamId;

    int fTypeId;

    double fRadius;

    ePointF fPos;
    eVec2d fVel;
    double fAngle;

    int fAnim;
    int fAnimId;
};

#endif // EUNITDATA_H

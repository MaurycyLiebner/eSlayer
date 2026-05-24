#ifndef ESKILLAREA_H
#define ESKILLAREA_H

#include "eslayerhelpersexport.h"

#include "epositioned.h"

struct ESLAYERHELPERS_API eSkillArea : public ePositioned {
    uint32_t fId;
    float fRadius;
    uint8_t fMissileId;
    float fRemTime;
};

#endif // ESKILLAREA_H

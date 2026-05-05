#ifndef EOBJECT_H
#define EOBJECT_H

#include "epositioned.h"

struct eObject : public ePositioned {
    uint32_t fObjectId;

    uint16_t fObjectType;
    uint8_t fSubtype;
    uint8_t fState = 0;

    float fSize;
};

#endif // EOBJECT_H

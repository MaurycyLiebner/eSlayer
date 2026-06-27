#ifndef EOBJECT_H
#define EOBJECT_H

#include "epositioned.h"

#include "eareaids.h"

struct eObject : public ePositioned {
    uint32_t fObjectId;

    uint16_t fObjectType;
    uint8_t fSubtype;
    uint8_t fState = 0;

    eAreaIds fTo;

    float fSize;
};

struct eServerObject : public eObject {
    eServerObject() {}
    eServerObject(const uint8_t mapId,
                  const eObject& obj) :
        eObject(obj), fMapId(mapId) {}

    uint8_t fMapId;
};

#endif // EOBJECT_H

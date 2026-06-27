#ifndef EAREAIDS_H
#define EAREAIDS_H

#include <cstdint>

struct eAreaIds {
    eAreaIds() {}
    eAreaIds(const uint8_t mapId,
             const uint8_t areaId) :
        fMapId(mapId),
        fAreaId(areaId) {}

    uint8_t fMapId;
    uint8_t fAreaId;

    bool operator==(const eAreaIds& other) const {
        if(fMapId != other.fMapId) return false;
        if(fAreaId != other.fAreaId) return false;
        return true;
    }

    bool operator!=(const eAreaIds& other) const {
        return !(*this == other);
    }
};

#endif // EAREAIDS_H

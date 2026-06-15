#ifndef EWAYPOINTS_H
#define EWAYPOINTS_H

#include <cstdint>
#include <vector>

struct eWaypoint {
    uint8_t fMapId;
    uint8_t fAreaId;
    bool fKnown;

    static bool known(const uint8_t mapId,
                      const uint8_t areaId);
    static bool setKnown(const uint8_t mapId,
                         const uint8_t areaId);

    static std::vector<eWaypoint> sWaypoints;
};

#endif // EWAYPOINTS_H

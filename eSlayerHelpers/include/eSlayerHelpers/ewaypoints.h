#ifndef EWAYPOINTS_H
#define EWAYPOINTS_H

#include <cstdint>
#include <vector>

#include "eobject.h"

struct eWaypoint {
    uint8_t fActId;
    eAreaIds fArea;
    bool fKnown;

    static bool known(const eAreaIds& area);
    static bool setKnown(const eAreaIds& area);

    static std::vector<eWaypoint> sWaypoints;
};

#endif // EWAYPOINTS_H

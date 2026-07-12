#ifndef EWAYPOINTS_H
#define EWAYPOINTS_H

#include "eslayerhelpersexport.h"

#include "eareaids.h"

#include <cstdint>
#include <vector>

struct ESLAYERHELPERS_API eWaypoint {
    uint8_t fActId;
    eAreaIds fArea;
    bool fKnown;

    static bool known(const eAreaIds& area);
    static bool setKnown(const eAreaIds& area);

    static std::vector<eWaypoint> sWaypoints;
};

#endif // EWAYPOINTS_H

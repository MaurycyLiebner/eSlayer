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
};

struct ESLAYERHELPERS_API eWaypoints :
        public std::vector<eWaypoint> {
    static bool known(const eAreaIds& area);
    static bool setKnown(const eAreaIds& area);

    static eWaypoints sWaypoints;

    void initialize();
};

#endif // EWAYPOINTS_H

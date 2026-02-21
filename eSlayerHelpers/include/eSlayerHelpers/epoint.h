#ifndef EPOINT_H
#define EPOINT_H

#include "eslayerhelpersexport.h"

#include <cmath>

struct ESLAYERHELPERS_API ePoint {
    int fX;
    int fY;
};

inline bool operator==(const ePoint& p1, const ePoint& p2) {
    return p1.fX == p2.fX && p1.fY == p2.fY;
}

inline bool operator!=(const ePoint& p1, const ePoint& p2) {
    return !operator==(p1, p2);
}

struct ESLAYERHELPERS_API ePointF {
    double fX;
    double fY;

    ePoint round() const {
        return {int(std::round(fX)),
                int(std::round(fY))};
    }
};

#endif // EPOINT_H

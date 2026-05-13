#ifndef EARCINTERVALS_H
#define EARCINTERVALS_H

#include "eslayerhelpersexport.h"

#include <vector>

struct eArcInterval {
    eArcInterval(const float start, const float end) :
        fAngleStart(start), fAngleEnd(end) {}
    float fAngleStart;
    float fAngleEnd;
};

struct ESLAYERHELPERS_API eArcIntervals :
    public std::vector<eArcInterval> {
    using std::vector<eArcInterval>::vector;
    bool angleInRange(const float angle) const;
    void subtract(float minAngleDeg,
                  float maxAngleDeg);
};

#endif // EARCINTERVALS_H

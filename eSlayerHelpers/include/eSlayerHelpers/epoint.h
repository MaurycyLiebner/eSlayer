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
    ePointF() {}
    ePointF(const double x, const double y) :
        fX(x), fY(y) {}
    ePointF(const ePoint& p) :
        fX(p.fX), fY(p.fY) {}

    double fX;
    double fY;

    ePoint round() const {
        return {int(std::round(fX)),
                int(std::round(fY))};
    }

    ePointF operator*(const double mult) const {
        return {mult*fX, mult*fY};
    }

    ePointF& operator*=(const double mult) {
        fX *= mult;
        fY *= mult;
        return *this;
    }
};

#endif // EPOINT_H

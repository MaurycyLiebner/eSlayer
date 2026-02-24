#ifndef EPOINT_H
#define EPOINT_H

#include "eslayerhelpersexport.h"

#include "evec2.h"

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

    static double distance(const ePointF& p1, const ePointF& p2);

    ePoint round() const {
        return {int(std::round(fX)),
                int(std::round(fY))};
    }

    ePoint floor() const {
        return {int(std::floor(fX)),
                int(std::floor(fY))};
    }

    ePointF operator+(const eVec2d& vec) const {
        return {fX + vec.x, fY + vec.y};
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

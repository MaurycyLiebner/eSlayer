#ifndef EPOINT_H
#define EPOINT_H

#include "eslayerhelpersexport.h"

#include "evec2.h"

#include <cmath>

struct ESLAYERHELPERS_API ePoint {
    int fX;
    int fY;

    ePoint operator/(const int div) const {
        return {fX/div, fY/div};
    }
};

inline bool operator==(const ePoint& p1, const ePoint& p2) {
    return p1.fX == p2.fX && p1.fY == p2.fY;
}

inline bool operator!=(const ePoint& p1, const ePoint& p2) {
    return !operator==(p1, p2);
}

struct ESLAYERHELPERS_API ePointF {
    ePointF() {}
    ePointF(const float x, const float y) :
        fX(x), fY(y) {}
    ePointF(const ePoint& p) :
        fX(p.fX), fY(p.fY) {}

    float fX;
    float fY;

    static float distance(const ePointF& p1, const ePointF& p2);

    static eVec2f vector(const ePointF& to, const ePointF& from);

    ePoint round() const {
        return {int(std::round(fX)),
                int(std::round(fY))};
    }

    ePoint floor() const {
        return {int(std::floor(fX)),
                int(std::floor(fY))};
    }

    ePointF operator+(const eVec2f& vec) const {
        return {fX + vec.x, fY + vec.y};
    }

    ePointF operator*(const float mult) const {
        return {mult*fX, mult*fY};
    }

    ePointF& operator*=(const float mult) {
        fX *= mult;
        fY *= mult;
        return *this;
    }
};

#endif // EPOINT_H

#ifndef ERECT_H
#define ERECT_H

#include "eslayerhelpersexport.h"

#include "epoint.h"

struct ESLAYERHELPERS_API eRect {
    int fX;
    int fY;
    int fW;
    int fH;

    int centerX() const { return fX + fW / 2; }
    int centerY() const { return fY + fH / 2; }

    bool contains(const ePoint& p) const {
        return p.fX >= fX && p.fX < fX + fW &&
               p.fY >= fY && p.fY < fY + fH;
    }

    static bool intersects(const eRect& a, const eRect& b) {
        return !(a.fX + a.fW < b.fX || b.fX + b.fW < a.fX ||
                 a.fY + a.fH < b.fY || b.fY + b.fH < a.fY);
    }

    void sum(const eRect& other) {
        const int left   = std::min(fX, other.fX);
        const int top    = std::min(fY, other.fY);
        const int right  = std::max(fX + fW, other.fX + other.fW);
        const int bottom = std::max(fY + fH, other.fY + other.fH);

        fX = left;
        fY = top;
        fW = right - left;
        fH = bottom - top;
    }
};

#endif // ERECT_H

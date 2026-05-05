#ifndef ERECT_H
#define ERECT_H

#include "eslayerhelpersexport.h"

#include "epoint.h"

struct ESLAYERHELPERS_API eRect {
    eRect() {}
    eRect(const int x, const int y,
          const int w, const int h) :
        fX(x), fY(y), fW(w), fH(h) {}

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

    static bool intersection(const eRect& a, const eRect& b, eRect& out) {
        const int left   = std::max(a.fX, b.fX);
        const int top    = std::max(a.fY, b.fY);
        const int right  = std::min(a.fX + a.fW, b.fX + b.fW);
        const int bottom = std::min(a.fY + a.fH, b.fY + b.fH);

        if(left < right && top < bottom) {
            out.fX = left;
            out.fY = top;
            out.fW = right - left;
            out.fH = bottom - top;
            return true;
        }

        // No intersection
        out = eRect(0, 0, 0, 0);
        return false;
    }

    eRect inset(const int by) const {
        eRect result = *this;
        result.fX += by;
        result.fY += by;
        result.fW -= 2*by;
        result.fH -= 2*by;
        return result;
    }
};

#endif // ERECT_H

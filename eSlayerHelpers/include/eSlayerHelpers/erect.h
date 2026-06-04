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

    bool contains(const ePoint& p) const;

    static bool intersects(const eRect& a, const eRect& b);

    void sum(const eRect& other);

    static bool intersection(const eRect& a, const eRect& b, eRect& out);

    eRect inset(const int by) const;

    static std::vector<eRect> subtract(
        const eRect &a, const eRect &b);
    static std::vector<eRect> subtractAll(
        const eRect &A, const std::vector<eRect> &B);
};

struct ESLAYERHELPERS_API eRectF {
    eRectF() {}
    eRectF(const float x, const float y,
           const float w, const float h) :
        fX(x), fY(y), fW(w), fH(h) {}

    float fX;
    float fY;
    float fW;
    float fH;

    bool lineIntersects(const ePointF& p1, const ePointF& p2) const;
    bool inside(const ePointF& p) const;
};

#endif // ERECT_H

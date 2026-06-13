#include "eSlayerHelpers/erect.h"

bool eRect::contains(const ePoint& p) const {
    return p.fX >= fX &&
           p.fX < fX + fW &&
           p.fY >= fY &&
           p.fY < fY + fH;
}

bool eRect::intersects(const eRect& a, const eRect& b) {
    return !(a.fX + a.fW < b.fX ||
             b.fX + b.fW < a.fX ||
             a.fY + a.fH < b.fY ||
             b.fY + b.fH < a.fY);
}

void eRect::sum(const eRect& other) {
    const int left = std::min(fX, other.fX);
    const int top = std::min(fY, other.fY);
    const int right = std::max(fX + fW, other.fX + other.fW);
    const int bottom = std::max(fY + fH, other.fY + other.fH);

    fX = left;
    fY = top;
    fW = right - left;
    fH = bottom - top;
}

bool eRect::intersection(
    const eRect& a, const eRect& b, eRect& out) {
    const int left = std::max(a.fX, b.fX);
    const int top = std::max(a.fY, b.fY);
    const int right = std::min(a.fX + a.fW, b.fX + b.fW);
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

eRect eRect::inset(const int by) const {
    eRect result = *this;
    result.fX += by;
    result.fY += by;
    result.fW -= 2 * by;
    result.fH -= 2 * by;
    return result;
}

std::vector<eRect> eRect::subtract(
    const eRect& a, const eRect& b) {
    std::vector<eRect> result;
    eRect inter;
    if(!intersection(a, b, inter)) {
        result.push_back(a);
        return result;
    }

    // Top rectangle
    if(inter.fY > a.fY) {
        result.push_back({a.fX, a.fY, a.fW, inter.fY - a.fY});
    }
    // Bottom rectangle
    if(inter.fY + inter.fH < a.fY + a.fH) {
        result.push_back({a.fX, inter.fY + inter.fH, a.fW, (a.fY + a.fH) - (inter.fY + inter.fH)});
    }
    // Left rectangle
    if(inter.fX > a.fX) {
        result.push_back({a.fX, inter.fY, inter.fX - a.fX, inter.fH});
    }
    // Right rectangle
    if(inter.fX + inter.fW < a.fX + a.fW) {
        result.push_back({inter.fX + inter.fW, inter.fY, (a.fX + a.fW) - (inter.fX + inter.fW), inter.fH});
    }

    return result;
}

std::vector<eRect> eRect::subtractAll(
    const eRect& A, const std::vector<eRect>& B) {
    std::vector<eRect> current;
    current.push_back(A);

    for(const auto& b : B) {
        std::vector<eRect> next;
        for(const auto& r : current) {
            std::vector<eRect> diff = subtract(r, b);
            next.insert(next.end(), diff.begin(), diff.end());
        }
        current = std::move(next);
    }

    return current;
}

bool pointInRect(
    const eRectF& r, const ePointF& p) {
    return p.fX >= r.fX &&
           p.fY >= r.fY &&
           p.fX <= r.fX + r.fW &&
           p.fY <= r.fY + r.fH;
}

float cross(
    const ePointF& a, const ePointF& b,
    const ePointF& c) {
    return (b.fX - a.fX) * (c.fY - a.fY) -
           (b.fY - a.fY) * (c.fX - a.fX);
}

bool segmentsIntersect(
    const ePointF& a1,
    const ePointF& a2,
    const ePointF& b1,
    const ePointF& b2) {
    const float d1 = cross(a1, a2, b1);
    const float d2 = cross(a1, a2, b2);
    const float d3 = cross(b1, b2, a1);
    const float d4 = cross(b1, b2, a2);

    return ((d1 > 0.f && d2 < 0.f) ||
            (d1 < 0.f && d2 > 0.f)) &&
           ((d3 > 0.f && d4 < 0.f) ||
            (d3 < 0.f && d4 > 0.f));
}

bool eRectF::lineIntersects(
    const ePointF& p1, const ePointF& p2) const {
    // Entirely inside
    if(pointInRect(*this, p1) ||
       pointInRect(*this, p2)) {
        return true;
    }

    const float x1 = fX;
    const float y1 = fY;
    const float x2 = fX + fW;
    const float y2 = fY + fH;

    // Rectangle edges
    const ePointF tl{x1, y1};
    const ePointF tr{x2, y1};
    const ePointF br{x2, y2};
    const ePointF bl{x1, y2};

    // Check against all 4 edges
    if(segmentsIntersect(p1, p2, tl, tr)) return true;
    if(segmentsIntersect(p1, p2, tr, br)) return true;
    if(segmentsIntersect(p1, p2, br, bl)) return true;
    if(segmentsIntersect(p1, p2, bl, tl)) return true;

    return false;
}

bool eRectF::inside(const ePointF& p) const {
    return p.fX >= fX && p.fX < fX + fW &&
           p.fY >= fY && p.fY < fY + fH;
}

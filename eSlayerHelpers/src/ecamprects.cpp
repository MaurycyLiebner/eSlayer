#include "eSlayerHelpers/ecamprects.h"

bool eCampRects::campAt(const ePointF& pos) const {
    for(const auto& c : mRects) {
        const bool r = c.inside(pos);
        if(r) return true;
    }
    return false;
}

bool eCampRects::campAtLine(
    const ePointF& from,
    const ePointF& to) const {
    for(const auto& c : mRects) {
        const bool r = c.lineIntersects(from, to);
        if(r) return true;
    }
    return false;
}

void eCampRects::addRect(const eRectF& rect) {
    mRects.emplace_back(rect);
}
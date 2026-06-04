#include "../include/eSlayerMapGenerator/echamber.h"

eChamber::eChamber() {}

eChamber::eChamber(const eRect& r) :
    fRects{r} {}

eChamber::eChamber(const std::vector<eRect>& r) :
    fRects{r} {}

int eChamber::area() const {
    int result = 0;
    for(const auto& r : fRects) {
        result += r.fW*r.fH;
    }
    return result;
}

bool eChamber::contains(const ePoint& p) const {
    for(const auto& r : fRects) {
        const bool c = r.contains(p);
        if(c) return true;
    }
    return false;
}

bool eChamber::wallTL(const ePoint& p) const {
    return contains(p) != contains({p.fX - 1, p.fY});
}

bool eChamber::wallTR(const ePoint& p) const {
    return contains(p) != contains({p.fX, p.fY - 1});
}

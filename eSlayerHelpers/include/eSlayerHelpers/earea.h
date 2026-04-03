#ifndef EAREA_H
#define EAREA_H

#include "epoint.h"

struct eArea : public ePoint {
    friend bool operator<(const eArea& t1, const eArea& t2) {
        if(t1.fY != t2.fY) return t1.fY < t2.fY;
        return t1.fX < t2.fX;
    }

    bool operator==(const eArea& other) const {
        return fX == other.fX && fY == other.fY;
    }
};

#endif // EAREA_H

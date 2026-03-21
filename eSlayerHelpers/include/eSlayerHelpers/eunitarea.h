#ifndef EUNITAREA_H
#define EUNITAREA_H

#include "epoint.h"

struct eUnitArea : public ePoint {
    friend bool operator<(const eUnitArea& t1, const eUnitArea& t2) {
        if(t1.fY != t2.fY) return t1.fY < t2.fY;
        return t1.fX < t2.fX;
    }
};

#endif // EUNITAREA_H

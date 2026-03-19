#ifndef EUNITTILE_H
#define EUNITTILE_H

#include "epoint.h"

struct eUnitTile : public ePoint {
    friend bool operator<(const eUnitTile& t1, const eUnitTile& t2) {
        if(t1.fY != t2.fY) return t1.fY < t2.fY;
        return t1.fX < t2.fX;
    }
};

#endif // EUNITTILE_H

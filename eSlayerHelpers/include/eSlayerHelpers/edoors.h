#ifndef EDOORS_H
#define EDOORS_H

#include "ewalldirection.h"

struct eDoors {
    eWallType fType;
    int fX;
    int fY;
    bool fOpen;
};

#endif // EDOORS_H

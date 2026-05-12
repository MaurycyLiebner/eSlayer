#ifndef EDOORS_H
#define EDOORS_H

#include "eslayerhelpersexport.h"

#include "ewalldirection.h"
#include "epoint.h"

#include <vector>

struct eDoorsTile {
    int fX;
    int fY;
};

struct ESLAYERHELPERS_API eDoors {
    eWallType fType;
    std::vector<eDoorsTile> fTiles;
    bool fOpen;

    ePointF pos() const;
};

#endif // EDOORS_H

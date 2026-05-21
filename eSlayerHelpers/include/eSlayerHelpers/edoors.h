#ifndef EDOORS_H
#define EDOORS_H

#include "eslayerhelpersexport.h"

#include "epacket.h"
#include "ewalldirection.h"
#include "epoint.h"

#include <vector>

struct ESLAYERHELPERS_API eDoors {
    eWallType fType;
    std::vector<ePoint> fTiles;
    bool fOpen;

    ePointF pos() const;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

#endif // EDOORS_H

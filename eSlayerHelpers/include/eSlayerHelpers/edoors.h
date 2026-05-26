#ifndef EDOORS_H
#define EDOORS_H

#include "eslayerhelpersexport.h"

#include "epacket.h"
#include "ewalldirection.h"
#include "epoint.h"

#include <vector>

struct ESLAYERHELPERS_API eDoorsStairsBase {
    eWallType fType;
    std::vector<ePoint> fTiles;

    ePointF pos() const;

    virtual void read(ePacket& p);
    virtual void write(ePacket& p) const;
};

struct ESLAYERHELPERS_API eStairs :
    public eDoorsStairsBase {
    uint8_t fMapId;

    void read(ePacket& p) override;
    void write(ePacket& p) const override;
};

struct ESLAYERHELPERS_API eDoors :
    public eDoorsStairsBase  {
    bool fOpen;

    void read(ePacket& p) override;
    void write(ePacket& p) const override;
};

#endif // EDOORS_H

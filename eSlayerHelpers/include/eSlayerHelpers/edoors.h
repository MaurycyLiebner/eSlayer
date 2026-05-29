#ifndef EDOORS_H
#define EDOORS_H

#include "eslayerhelpersexport.h"

#include "epacket.h"
#include "ewalldirection.h"
#include "epoint.h"

#include <vector>

struct ESLAYERHELPERS_API eDoorsStairsBase {
    eDoorsStairsBase();
    eDoorsStairsBase(const eWallType wallType,
                     const int type, const int nTypes,
                     const int x0, const int y0);

    eWallType fType;
    std::vector<ePoint> fTiles;

    ePointF pos() const;

    virtual void read(ePacket& p);
    virtual void write(ePacket& p) const;
};

struct ESLAYERHELPERS_API eStairs :
    public eDoorsStairsBase {
    eStairs();
    eStairs(const eWallType wallType,
            const int type, const int nTypes,
            const int x0, const int y0,
            const uint8_t mapId);

    uint8_t fMapId;

    void read(ePacket& p) override;
    void write(ePacket& p) const override;
};

struct ESLAYERHELPERS_API eDoors :
    public eDoorsStairsBase {
    eDoors();
    eDoors(const eWallType wallType,
           const int type, const int nTypes,
           const int x0, const int y0,
           const bool open);

    bool fOpen;

    void read(ePacket& p) override;
    void write(ePacket& p) const override;
};

#endif // EDOORS_H

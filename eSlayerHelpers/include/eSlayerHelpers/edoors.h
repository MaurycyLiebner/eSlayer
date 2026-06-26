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
protected:
    virtual void read(ePacket& p);
    virtual void write(ePacket& p) const;
};

struct ESLAYERHELPERS_API eStairs :
    public eDoorsStairsBase {
    eStairs();
    eStairs(const eWallType wallType,
            const int type, const int nTypes,
            const int x0, const int y0,
            const uint8_t targetMapId);

    uint8_t fTargetMapId;
    uint8_t fTargetAreaId;
};

struct ESLAYERHELPERS_API eDoors :
    public eDoorsStairsBase {
    eDoors();
    eDoors(const eWallType wallType,
           const int type, const int nTypes,
           const int x0, const int y0,
           const bool open);

    bool fOpen;
};

struct ESLAYERHELPERS_API eWithMapId {
    eWithMapId() {}
    eWithMapId(const uint8_t mapId) :
        fMapId(mapId) {}

    uint8_t fMapId;
};

struct ESLAYERHELPERS_API eServerStairs :
    public eStairs,
    public eWithMapId {
    eServerStairs();
    eServerStairs(const uint8_t mapId,
                  const eStairs& stairs);

    void read(ePacket& p) override;
    void write(ePacket& p) const override;
};

struct ESLAYERHELPERS_API eServerDoors :
    public eDoors,
    public eWithMapId {
    eServerDoors();
    eServerDoors(const uint8_t mapId,
                 const eDoors& doors);

    void read(ePacket& p) override;
    void write(ePacket& p) const override;
};

#endif // EDOORS_H

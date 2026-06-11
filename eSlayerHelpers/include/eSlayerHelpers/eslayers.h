#ifndef ESLAYERS_H
#define ESLAYERS_H

#include "eslayerhelpersexport.h"

#include <cstdint>
#include <string>
#include <map>

#include "epoint.h"

class ePacket;

struct eSlayerLocation {
    uint32_t fClientId;
    uint8_t fMapId;
    ePointF fPos;
};

struct ESLAYERHELPERS_API eSlayer :
    public eSlayerLocation {
    std::string fName;

    void write(ePacket& p) const;
    void read(ePacket& p);
};

struct ESLAYERHELPERS_API eSlayers {
    static uint32_t sThisSlayer;
    static std::map<uint32_t, eSlayer> sSlayers;
    static std::string name(const uint32_t clientId);
    static bool setLocation(const uint32_t clientId,
                            const uint8_t mapId,
                            const ePointF& pos);
    static bool setPoisition(const uint32_t clientId,
                             const ePointF& pos);
    static void writeLocations(ePacket& p);
    static void readLocations(ePacket& p);
    static void write(ePacket& p);
    static void read(ePacket& p);
    static void clear();
};

#endif // ESLAYERS_H

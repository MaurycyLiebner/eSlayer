#ifndef ESLAYERS_H
#define ESLAYERS_H

#include "eslayerhelpersexport.h"

#include "epoint.h"
#include "eteamid.h"

#include <cstdint>
#include <string>
#include <map>

class ePacket;

struct ESLAYERHELPERS_API eSlayer {
    uint32_t fClientId;
    std::string fName;
    ePointF fPos;
    uint8_t fMapId = 0;
    uint8_t fAreaId = 0;
    uint16_t fHealth = 100;
    uint16_t fMaxHealth = 100;
    eTeamId fTeamId = eTeamId::playerTeam0;

    void write(ePacket& p) const;
    void read(ePacket& p);
};

struct ESLAYERHELPERS_API eSlayers {
    static uint32_t sThisSlayer;
    static std::map<uint32_t, eSlayer> sSlayers;
    static std::string name(const uint32_t clientId);
    static void write(ePacket& p);
    static void read(ePacket& p);
    static void clear();
};

#endif // ESLAYERS_H

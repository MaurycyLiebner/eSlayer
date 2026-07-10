#ifndef ESLAYERS_H
#define ESLAYERS_H

#include "eslayerhelpersexport.h"

#include "eunitspecialdata.h"
#include "eteamid.h"

#include <cstdint>
#include <string>
#include <map>

class ePacket;

struct ESLAYERHELPERS_API eSlayer :
    public eUnitSpecialData {
    std::string fName;
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

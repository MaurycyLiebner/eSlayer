#ifndef ETEAMID_H
#define ETEAMID_H

#include "eslayerhelpersexport.h"

#include <cstdint>

enum class eTeamId : uint8_t {
    neutral,
    neutralHostile,
    neutralFriendly,
    playerTeam0
};

#include <map>
#include <set>

class ePacket;

class ESLAYERHELPERS_API eTeams {
public:
    static bool areEnemies(
        const eTeamId team1, const eTeamId team2);

    static void makeEnemies(
        const eTeamId team1, const eTeamId team2);

    static void makeFriends(
        const eTeamId team1, const eTeamId team2);

    static void read(ePacket& p);
    static void write(ePacket& p);

    static uint16_t version()
    { return sVersion; }
private:
    static uint16_t sVersion;
    static std::map<eTeamId, std::set<eTeamId>> sEnemies;
};

#endif // ETEAMID_H

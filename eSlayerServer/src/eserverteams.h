#ifndef ESERVERTEAMS_H
#define ESERVERTEAMS_H

#include <eSlayerHelpers/eteamid.h>

#include <map>
#include <set>

class eServerTeams {
public:
    static bool areEnemies(
        const eTeamId team1, const eTeamId team2);

    static void makeEnemies(
        const eTeamId team1, const eTeamId team2);

    static void makeFriends(
        const eTeamId team1, const eTeamId team2);

    static std::map<eTeamId, std::set<eTeamId>> sEnemies;
};

#endif // ESERVERTEAMS_H

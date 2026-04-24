#include "eserverteams.h"

std::map<eTeamId, std::set<eTeamId>>
eServerTeams::sEnemies;

bool eServerTeams::areEnemies(
    const eTeamId team1, const eTeamId team2) {
    if(team1 == team2) return false;
    if(team1 == eTeamId::neutralHostile ||
       team2 == eTeamId::neutralHostile) return true;
    if(team1 == eTeamId::neutralFriendly ||
       team2 == eTeamId::neutralFriendly) return false;
    const auto it = sEnemies.find(team1);
    if(it == sEnemies.end()) return false;
    const auto& set = it->second;
    return set.find(team2) != set.end();
}

void eServerTeams::makeEnemies(
    const eTeamId team1, const eTeamId team2) {
    sEnemies[team1].emplace(team2);
    sEnemies[team2].emplace(team1);
}

void eServerTeams::makeFriends(
    const eTeamId team1, const eTeamId team2) {
    sEnemies[team1].erase(team2);
    sEnemies[team2].erase(team1);
}

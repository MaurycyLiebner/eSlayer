#include "eSlayerHelpers/eteamid.h"

#include "eSlayerHelpers/epacket.h"

uint16_t eTeams::sVersion = 0;
std::map<eTeamId, std::set<eTeamId>>
eTeams::sEnemies;

bool eTeams::areEnemies(
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

void eTeams::makeEnemies(
    const eTeamId team1, const eTeamId team2) {
    sEnemies[team1].emplace(team2);
    sEnemies[team2].emplace(team1);
    sVersion++;
}

void eTeams::makeFriends(
    const eTeamId team1, const eTeamId team2) {
    sEnemies[team1].erase(team2);
    sEnemies[team2].erase(team1);
    sVersion++;
}

void eTeams::read(ePacket& p) {
    sEnemies.clear();
    uint8_t nTeams;
    p >> nTeams;
    for(int i = 0; i < nTeams; i++) {
        eTeamId t1;
        p >> t1;
        auto& set = sEnemies[t1];
        uint8_t nEnemies;
        p >> nEnemies;
        for(int j = 0; j < nEnemies; j++) {
            eTeamId t2;
            p >> t2;
            set.emplace(t2);
        }
    }
    p >> sVersion;
}

void eTeams::write(ePacket& p) {
    const uint8_t nTeams = sEnemies.size();
    p << nTeams;
    for(const auto& it : sEnemies) {
        const eTeamId t1 = it.first;
        p << t1;
        const auto& set = it.second;
        const uint8_t nEnemies = set.size();
        p << nEnemies;
        for(const eTeamId t2 : set) {
            p << t2;
        }
    }
    p << sVersion;
}

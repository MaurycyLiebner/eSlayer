#include "eSlayerHelpers/eteamid.h"

#include "eSlayerHelpers/epacket.h"

uint16_t eTeams::sVersion = 0;
std::map<eTeamId, eTeam>
eTeams::sTeams;

bool eTeams::areEnemies(
    const eTeamId teamId1, const eTeamId teamId2) {
    if(teamId1 == teamId2) return false;
    if(teamId1 == eTeamId::neutral ||
       teamId2 == eTeamId::neutral) return false;
    if(teamId1 == eTeamId::neutralHostile ||
       teamId2 == eTeamId::neutralHostile) return true;
    if(teamId1 == eTeamId::neutralFriendly ||
       teamId2 == eTeamId::neutralFriendly) return false;
    const auto it = sTeams.find(teamId1);
    if(it == sTeams.end()) return false;
    const auto& team = it->second;
    const auto& set = team.fEnemies;
    return set.find(teamId2) != set.end();
}

bool eTeams::makeEnemies(
    const eTeamId teamId, const uint32_t clientId) {
    const auto cteamId = playerTeam(clientId);
    return makeEnemies(teamId, cteamId);
}

bool eTeams::makeFriends(
    const eTeamId teamId, const uint32_t clientId) {
    const auto cteamId = playerTeam(clientId);
    return makeFriends(teamId, cteamId);
}

bool eTeams::makeEnemies(
    const eTeamId teamId1, const eTeamId teamId2) {
    const auto it1 = sTeams.find(teamId1);
    if(it1 == sTeams.end()) return false;

    const auto it2 = sTeams.find(teamId2);
    if(it2 == sTeams.end()) return false;

    auto& team1 = it1->second;
    auto& set1 = team1.fEnemies;
    set1.emplace(teamId2);

    auto& team2 = it2->second;
    auto& set2 = team2.fEnemies;
    set2.emplace(teamId1);

    sVersion++;
    return true;
}

bool eTeams::makeFriends(
    const eTeamId teamId1, const eTeamId teamId2) {
    const auto it1 = sTeams.find(teamId1);
    if(it1 == sTeams.end()) return false;

    const auto it2 = sTeams.find(teamId2);
    if(it2 == sTeams.end()) return false;

    auto& team1 = it1->second;
    auto& set1 = team1.fEnemies;
    set1.erase(teamId2);

    auto& team2 = it2->second;
    auto& set2 = team2.fEnemies;
    set2.erase(teamId1);

    sVersion++;
    return true;
}

bool eTeams::invite(
    const uint32_t invited, const uint32_t clientId) {
    const auto toTeam = playerTeam(clientId);
    return invite(toTeam, invited);
}

bool eTeams::invite(
    const eTeamId teamId, const uint32_t invited) {

    const auto cteamId = playerTeam(invited);
    const auto cit = sTeams.find(cteamId);
    if(cit == sTeams.end()) return false;
    auto& cteam = cit->second;
    auto& cmems = cteam.fMembers;
    if(cmems.size() > 1) return false;

    const auto it = sTeams.find(teamId);
    if(it == sTeams.end()) return false;
    auto& team = it->second;
    team.fInvitations.emplace(invited);
    return true;
}

bool eTeams::acceptInvitation(
    const eTeamId teamId, const uint32_t clientId) {
    const auto it = sTeams.find(teamId);
    if(it == sTeams.end()) return false;
    auto& team = it->second;
    auto& invs = team.fInvitations;
    if(invs.find(clientId) == invs.end()) return false;
    const auto cteam = playerTeam(clientId);
    removeMember(cteam, clientId);
    clearInvitationsFor(clientId);
    return addMember(teamId, clientId);
}

eTeamId eTeams::leaveTeam(
    const uint32_t clientId) {
    const auto team = playerTeam(clientId);
    removeMember(team, clientId);
    clearInvitationsFor(clientId);
    return addTeam(clientId);
}

eTeamId eTeams::addTeam(const uint32_t clientId) {
    eTeamId result{eTeamId::playerTeam0};
    while(sTeams.find(result) != sTeams.end()) {
        const int inext = static_cast<int>(result) + 1;
        result = static_cast<eTeamId>(inext);
    }
    addMember(result, clientId);
    clearInvitationsFor(clientId);
    sVersion++;
    return result;
}

bool eTeams::addMember(
    const eTeamId teamId, const uint32_t clientId) {
    const auto it = sTeams.find(teamId);
    if(it == sTeams.end()) return false;
    auto& team = it->second;
    team.fMembers.emplace(clientId);
    clearInvitationsFor(clientId);
    sVersion++;
    return true;
}

bool eTeams::removeMember(
    const eTeamId teamId, const uint32_t clientId) {
    const auto it = sTeams.find(teamId);
    if(it == sTeams.end()) return false;
    auto& team = it->second;
    auto& members = team.fMembers;
    members.erase(clientId);
    if(members.empty()) {
        sTeams.erase(it);
    }
    clearInvitationsFor(clientId);
    sVersion++;
    return true;
}

eTeamId eTeams::playerTeam(const uint32_t clientId) {
    for(const auto& it : sTeams) {
        const auto teamId = it.first;
        const auto& team = it.second;
        const auto& ms = team.fMembers;
        if(ms.find(clientId) == ms.end()) continue;
        return teamId;
    }
    return eTeamId::neutral;
}

void eTeams::read(ePacket& p) {
    uint8_t nTeams;
    p >> nTeams;
    for(int i = 0; i < nTeams; i++) {
        eTeamId teamId;
        p >> teamId;
        eTeam team;
        team.read(p);
        sTeams[teamId] = team;
    }
    p >> sVersion;
}

void eTeams::write(ePacket& p) {
    const uint8_t nTeams = sTeams.size();
    p << nTeams;
    for(const auto& it : sTeams) {
        const auto teamId = it.first;
        p << teamId;
        const auto& team = it.second;
        team.write(p);
    }
    p << sVersion;
}

void eTeams::clear() {
    sVersion = 0;
    sTeams.clear();
}

void eTeams::clearInvitationsFor(const uint32_t clientId) {
    for(auto& it : sTeams) {
        auto& team = it.second;
        auto& invs = team.fInvitations;
        invs.erase(clientId);
    }
}

void eTeam::read(ePacket& p) {
    p.read8(fEnemies);
    p.read8(fMembers);
    p.read8(fInvitations);
}

void eTeam::write(ePacket& p) const {
    p.write8(fEnemies);
    p.write8(fMembers);
    p.write8(fInvitations);
}

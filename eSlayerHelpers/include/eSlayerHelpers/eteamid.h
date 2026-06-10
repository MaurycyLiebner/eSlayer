#ifndef ETEAMID_H
#define ETEAMID_H

#include "eslayerhelpersexport.h"

#include <cstdint>

enum class eTeamActionType : uint8_t {
    makeEnemies,
    makeFriends,
    invite,
    cancelInvite,
    acceptInvitation,
    leaveTeam
};

enum class eTeamId : uint8_t {
    neutral,
    neutralHostile,
    neutralFriendly,
    playerTeam0
};

struct eTeamAction {
    eTeamActionType fType;
    eTeamId fTeamId;
    uint32_t fInvitedId;
};

#include <map>
#include <set>

class ePacket;

struct ESLAYERHELPERS_API eTeam {
    std::set<eTeamId> fEnemies;
    std::set<uint32_t> fMembers;
    std::set<uint32_t> fInvitations;

    void read(ePacket& p);
    void write(ePacket& p) const;
};

class ESLAYERHELPERS_API eTeams {
public:
    static bool areEnemies(
        const eTeamId teamId1, const eTeamId teamId2);

    static bool makeEnemies(
        const eTeamId teamId, const uint32_t clientId);
    static bool makeFriends(
        const eTeamId teamId, const uint32_t clientId);

    static bool invite(
        const uint32_t invited, const uint32_t clientId);
    static bool isInvited(
        const uint32_t invited, const uint32_t clientId);
    static bool cancelInvite(
        const uint32_t invited, const uint32_t clientId);

    static bool acceptInvitation(
        const eTeamId teamId, const uint32_t clientId);
    static eTeamId leaveTeam(
        const uint32_t clientId);

    static eTeamId addTeam(const uint32_t clientId);

    static eTeamId playerTeam(const uint32_t clientId);
    static const std::map<eTeamId, eTeam>& teams()
    { return sTeams; }

    static void read(ePacket& p);
    static void write(ePacket& p);

    static void clear();

    static uint16_t version()
    { return sVersion; }
private:
    static void clearInvitationsFor(const uint32_t clientId);
    static bool addMember(
        const eTeamId teamId, const uint32_t clientId);
    static bool removeMember(
        const eTeamId teamId, const uint32_t clientId);

    static bool invite(
        const eTeamId teamId, const uint32_t clientId);
    static bool isInvited(
        const eTeamId teamId, const uint32_t clientId);
    static bool cancelInvite(
        const eTeamId teamId, const uint32_t clientId);

    static bool makeEnemies(
        const eTeamId teamId1, const eTeamId teamId2);
    static bool makeFriends(
        const eTeamId teamId1, const eTeamId teamId2);

    static uint16_t sVersion;
    static std::map<eTeamId, eTeam> sTeams;
};

#endif // ETEAMID_H

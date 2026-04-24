#ifndef ETEAMID_H
#define ETEAMID_H

#include <cstdint>

enum class eTeamId : uint8_t {
    neutralHostile,
    neutralFriendly,
    playerTeam0
};

#endif // ETEAMID_H

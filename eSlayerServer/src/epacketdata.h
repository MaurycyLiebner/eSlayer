#ifndef EPACKETDATA_H
#define EPACKETDATA_H

#include <cstdint>

enum class ePacketType : uint8_t {
    connect, map, spawn,
    data, request, state,
    attack, stopAttack, respawn,
    setSkillId,
    pickupItem, dropItem, rearrangeItems,
    equipment,
    disconnect
};

#endif // EPACKETDATA_H

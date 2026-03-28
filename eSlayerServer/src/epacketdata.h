#ifndef EPACKETDATA_H
#define EPACKETDATA_H

#include <cstdint>

enum class ePacketType : uint8_t {
    connect, map, data, request, state,
    attack, stopAttack, respawn,
    setSkillId, weaponData,
    pickupItem, dropItem, rearrangeItems,
    disconnect
};

#endif // EPACKETDATA_H

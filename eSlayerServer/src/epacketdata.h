#ifndef EPACKETDATA_H
#define EPACKETDATA_H

#include <cstdint>

enum class ePacketType : uint8_t {
    connect, map, spawn,
    userEntered, userLeft,
    message,
    data, request, state,
    attack, stopAttack, respawn,
    setSkillId, consumePotion,
    pickupItem, dropItem,
    rearrangeItems, attributes,
    skills,
    equipment,
    disconnect
};

#endif // EPACKETDATA_H

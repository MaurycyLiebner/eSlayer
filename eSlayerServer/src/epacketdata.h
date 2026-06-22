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
    triggerObject, objectStateChanged,
    pickupItem, dropItem, dropGold,
    triggerDoors, doorsStateChanged,
    pickupBody,
    rearrangeItems, attributes,
    skills, teams, portals,
    equipment, unblockEquipment,
    equipmentAction, buyAction,
    newItemId,
    body, bodyPickedUp,
    teamsAction, spawnPortal,
    disconnect
};

#endif // EPACKETDATA_H

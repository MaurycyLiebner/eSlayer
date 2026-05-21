#include "etcpipjoin.h"

#include "epacketdata.h"

#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/escreendimensions.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/edoors.h>

eTcpIpJoin::eTcpIpJoin(const std::string& ip) :
    mIP(ip) {}

eTcpIpJoin::~eTcpIpJoin() {
    if(mInitialized) mNet.shutdown();
}

bool eTcpIpJoin::initialize() {
    mInitialized = mNet.init();
    if(!mInitialized) {
        failed("Disconnected", "Failed to initialize SDL3_net.");
        return false;
    }
    const bool r = mNet.connect(mIP.data(), 4000);
    if(!r) failed("Disconnected", "Failed to connect to the host.");
    return r;
}

int eTcpIpJoin::connect() {
    ePacket p;
    p << ePacketType::connect;
    const bool r = mNet.sendToServer(p);
    if(!r) {
        failed("Disconnected", "Failed to send connection request.");
        return -1;
    }
    uint32_t time = 0;
    while(true) {
        mNet.update();

        eNetPacket pkt;

        while(mNet.pollPacket(pkt)) {
            auto& p = pkt.fPacket;
            ePacketType type;
            p >> type;

            if(type == ePacketType::connect) {
                int32_t clientId;
                p >> clientId;
                return clientId;
            }
        }

        SDL_Delay(16);
        time += 16;
        if(time > 10000) {
            failed("Disconnected", "Connection timed out.");
            return -1;
        }
    }
}

bool eTcpIpJoin::disconnect(const int clientId) {
    ePacket p;
    p << ePacketType::disconnect;
    return mNet.sendToServer(p);
}

void eTcpIpJoin::increment(const float by) {
    mNet.update();
    eNetPacket pkt;
    while(mNet.pollPacket(pkt)) {
        auto& p = pkt.fPacket;
        ePacketType type;
        p >> type;
        switch(type) {
        case ePacketType::data: {
            mData.fUpdatedUnits.clear();
            mData.read(p);
            mNewData = true;
        } break;
        case ePacketType::equipment: {
            mEquipment = eEquipment();
            mEquipment.read(p);
            mNewEquipment = true;
        } break;
        case ePacketType::unblockEquipment: {
            mUnblockEquipment = true;
        } break;
        case ePacketType::userEntered: {
            int clientId;
            p >> clientId;
            std::string name;
            p >> name;
            mNewUsers.emplace_back(clientId, name, true);
        } break;
        case ePacketType::userLeft: {
            int clientId;
            p >> clientId;
            mLeftUsers.emplace_back(clientId);
        } break;
        case ePacketType::message: {
            int clientId;
            p >> clientId;
            std::string msg;
            p >> msg;
            mMessages.emplace_back(clientId, msg);
        } break;
        case ePacketType::teams: {
            eTeams::read(p);
        } break;
        case ePacketType::objectStateChanged: {
            eObject obj;
            p >> obj;
            mObjectStateChanges.emplace_back(obj);
        } break;
        case ePacketType::doorsStateChanged: {
            eDoors doors;
            doors.read(p);
            mDoorsStateChanged.emplace_back(doors);
        } break;
        case ePacketType::disconnect: {
            failed("Disconnected", "Host closed the connection.");
        } break;
        default:
            break;
        }
    }
}

bool eTcpIpJoin::requestMap(
    const int clientId,
    const std::string& name,
    eMapData& data) {
    ePacket p;
    p << ePacketType::map;
    p << name;
    const bool r = mNet.sendToServer(p);
    if(!r) {
        failed("Disconnected", "Failed to send map request to the host.");
        return false;
    }
    uint32_t time = 0;
    while(true) {
        mNet.update();

        eNetPacket pkt;

        while(mNet.pollPacket(pkt)) {
            auto& p = pkt.fPacket;
            ePacketType type;
            p >> type;

            if(type == ePacketType::map) {
                data.read(p);
                return true;
            }
        }

        SDL_Delay(16);
        time += 16;
        if(time > 10000) {
            failed("Disconnected", "Map request timed out.");
            return false;
        }
    }
}

bool eTcpIpJoin::spawn(
    const int clientId,
    eCharacter& c,
    eTeamId& teamId,
    const eScreenDimensions& screenDims) {
    ePacket p;
    p << ePacketType::spawn;
    c.write(p);
    screenDims.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) {
        failed("Disconnected", "Failed to send spawn request to the host.");
        return false;
    }
    uint32_t time = 0;
    while(true) {
        mNet.update();

        eNetPacket pkt;

        while(mNet.pollPacket(pkt)) {
            auto& p = pkt.fPacket;
            ePacketType type;
            p >> type;

            if(type == ePacketType::spawn) {
                uint8_t nClients;
                p >> nClients;
                for(uint8_t i = 0; i < nClients; i++) {
                    int clientId;
                    p >> clientId;
                    std::string name;
                    p >> name;
                    mNewUsers.emplace_back(clientId, name, false);
                }

                c = eCharacter();
                c.read(p);
                eTeams::read(p);
                p >> teamId;
                return true;
            }
        }

        SDL_Delay(16);
        time += 16;
        if(time > 10000) {
            failed("Disconnected", "Character request timed out.");
            return false;
        }
    }
    return true;
}

bool eTcpIpJoin::requestData(const int clientId,
                             eRequestData& data,
                             float& resultTime) {
    {
        ePacket p;
        p << ePacketType::request;
        p << mRequestId++;
        const bool r = mNet.sendToServer(p);
        if(!r) failed("Disconnected", "Failed to send a request to the host.");
    }
    if(!mNewData) return false;
    mReceivedId = mData.fRequestId;
    std::swap(mData, data);
    mNewData = false;
    return true;
}

bool eTcpIpJoin::requestEquipment(const int clientId) {
    ePacket p;
    p << ePacketType::equipment;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a request to the host.");
    return r;
}

bool eTcpIpJoin::receiveEquipment(
    const int clientId, eEquipment& data) {
    if(!mNewEquipment) return false;
    data = mEquipment;
    mNewEquipment = false;
    return true;
}

bool eTcpIpJoin::unblockEquipment(const int clientId) {
    const bool unblock = mUnblockEquipment;
    mUnblockEquipment = false;
    return unblock;
}

bool eTcpIpJoin::changeState(
    const int clientId, const eUnitData& u) {
    ePacket p;
    p << ePacketType::state;
    u.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send state change to the host.");
    return r;
}

bool eTcpIpJoin::attack(const int clientId,
                        const eAttackData& target) {
    ePacket p;
    p << ePacketType::attack;
    target.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send attack change to the host.");
    return true;
}

bool eTcpIpJoin::stopAttack(const int clientId) {
    ePacket p;
    p << ePacketType::stopAttack;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send attack change to the host.");
    return true;
}

bool eTcpIpJoin::respawn(const int clientId) {
    ePacket p;
    p << ePacketType::respawn;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send respawn request to the host.");
    return true;
}

bool eTcpIpJoin::setSkillId(const int clientId,
                            const eSkillChoice schoice,
                            const int skillId) {
    ePacket p;
    p << ePacketType::setSkillId;
    p << schoice;
    p << skillId;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send skill change to the host.");
    return true;
}

std::shared_ptr<eObject> eTcpIpJoin::triggerObject(
    const int clientId, const int objectId,
    const int tx, const int ty) {
    ePacket p;
    p << ePacketType::triggerObject;
    p << objectId;
    p << tx;
    p << ty;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send object trigger to the host.");
    return nullptr;
}

bool eTcpIpJoin::triggerDoors(const int clientId,
                              const eDoors& doors) {
    ePacket p;
    p << ePacketType::triggerDoors;
    doors.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send trigger doors to the host.");
    return true;
}

bool eTcpIpJoin::pickupItem(const int clientId,
                            const int itemId,
                            const bool drag) {
    ePacket p;
    p << ePacketType::pickupItem;
    p << itemId;
    p << drag;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send item pickup to the host.");
    return true;
}

bool eTcpIpJoin::dropItem(
    const int clientId) {
    ePacket p;
    p << ePacketType::dropItem;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send item drop to the host.");
    return true;
}

bool eTcpIpJoin::rearrangeItems(
    const int clientId, const eEquipment& eq) {
    ePacket p;
    p << ePacketType::rearrangeItems;
    eq.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send equipment rearrangement to the host.");
    return true;
}

bool eTcpIpJoin::changeAttributes(
    const int clientId, const eAttributes& attrs) {
    ePacket p;
    p << ePacketType::attributes;
    attrs.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send attributes change to the host.");
    return true;
}

bool eTcpIpJoin::changeSkillLevels(
    const int clientId, const eSkillLevels& skillLevels) {
    ePacket p;
    p << ePacketType::skills;
    skillLevels.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send skill levels change to the host.");
    return true;
}

bool eTcpIpJoin::sendMessage(
    const int clientId, const std::string& text) {
    ePacket p;
    p << ePacketType::message;
    p << text;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a message to the host.");
    return true;
}

bool eTcpIpJoin::consumePotion(
    const int clientId, const uint32_t itemId) {
    ePacket p;
    p << ePacketType::consumePotion;
    p << itemId;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a potion consumption to the host.");
    return true;
}

bool eTcpIpJoin::pickupBody(
    const int clientId, const int32_t bodyId) {
    ePacket p;
    p << ePacketType::pickupBody;
    p << bodyId;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a pickup body to the host.");
    return true;
}

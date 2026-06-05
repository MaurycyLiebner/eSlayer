#include "etcpiphost.h"

#include "epacketdata.h"

#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/edoors.h>

eTcpIpHost::~eTcpIpHost() {
    if(mInitialized) {
        ePacket p;
        p << ePacketType::disconnect;
        for(const auto& c : mClientIdMap) {
            const int tcpClientId = c.first;
            mNet.sendToClient(tcpClientId, p);
        }

        uint32_t time = 0;
        while(!mClientIdMap.empty()) {
            mNet.update();

            eNetPacket pkt;

            while(mNet.pollPacket(pkt)) {
                auto& p = pkt.fPacket;
                const int tcpClientId = pkt.fClientID;
                ePacketType type;
                p >> type;

                if(type == ePacketType::disconnect) {
                    mClientIdMap.erase(tcpClientId);
                }
            }

            SDL_Delay(16);
            time += 16;
            if(time > 2000) {
                break;
            }
        }

        mNet.shutdown();
    }
}

bool eTcpIpHost::initialize() {
    eLocalServer::initialize();
    mInitialized = mNet.init();
    if(!mInitialized) {
        failed("Disconnected", "Failed to initialize SDL3_net.");
        return false;
    }
    const bool r = mNet.startServer(4000);
    if(!r) failed("Disconnected", "Failed to create the host server.");
    return r;
}

void eTcpIpHost::increment(const float by) {
    eLocalServer::increment(by);
    mNet.update();
    eNetPacket pkt;

    if(eTeams::version() > mTeamsVersion) {
        ePacket p;
        p << ePacketType::teams;
        eTeams::write(p);
        mNet.broadcast(p);
    }

    while(mNet.pollPacket(pkt)) {
        const int tcpClientId = pkt.fClientID;
        auto& p = pkt.fPacket;
        ePacketType type;
        p >> type;
        switch(type) {
        case ePacketType::connect: {
            const auto it = mClientIdMap.find(tcpClientId);
            int32_t clientId;
            if(it == mClientIdMap.end()) {
                clientId = connect();
                mClientIdMap[tcpClientId] = clientId;
            } else {
                clientId = it->second;
            }
            ePacket p;
            p << ePacketType::connect;
            p << clientId;
            mNet.sendToClient(pkt.fClientID, p);
        } break;
        case ePacketType::map: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                std::string mapName;
                p >> mapName;
                const int charId = it->second;
                const auto func = [this, tcpClientId](const eMapData& data) {
                    const auto it = mClientIdMap.find(tcpClientId);
                    if(it == mClientIdMap.end()) return;
                    ePacket p;
                    p << ePacketType::map;
                    data.write(p);
                    mNet.sendToClient(tcpClientId, p);
                };
                requestMap(charId, mapName, func);
            }
        } break;
        case ePacketType::spawn: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                eCharacter c;
                c.read(p);
                eScreenDimensions screenDims;
                screenDims.read(p);
                eTeamId teamId;
                const bool r = spawn(charId, c, teamId, screenDims);

                if(r) {
                    {
                        ePacket p;
                        p << ePacketType::spawn;

                        const uint8_t nClients = mClientHandlers.size();
                        p << nClients;
                        for(const auto& it : mClientHandlers) {
                            const int clientId = it.first;
                            p << clientId;
                            const auto h = it.second;
                            const auto name = h ? h->name() : "";
                            p << name;
                        }

                        const auto& eq = c.equipment();
                        eq.write(p);

                        for(auto& b : c.bodies()) {
                            p << b.fBodyId;
                        }

                        eTeams::write(p);
                        p << teamId;
                        mNet.sendToClient(pkt.fClientID, p);
                    }

                    {
                        ePacket p;
                        p << ePacketType::userEntered;
                        p << charId;
                        const auto name = c.name();
                        p << name;
                        mNet.broadcast(p);

                        mNewUsers.emplace_back(charId, name, true);
                    }
                }
            }
        } break;
        case ePacketType::state: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                eUnitData u;
                u.read(p);
                changeState(charId, u);
            }
        } break;
        case ePacketType::data: {

        } break;
        case ePacketType::userEntered: {

        } break;
        case ePacketType::userLeft: {

        } break;
        case ePacketType::unblockEquipment: {

        } break;
        case ePacketType::equipment: {

        } break;
        case ePacketType::body: {

        } break;
        case ePacketType::bodyPickedUp: {

        } break;
        case ePacketType::teams: {

        } break;
        case ePacketType::objectStateChanged: {

        } break;
        case ePacketType::doorsStateChanged: {

        } break;
        case ePacketType::message: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                std::string msg;
                p >> msg;
                sendMessageToAll(charId, msg);
            }
        } break;
        case ePacketType::request: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                eRequestData data;
                p >> data.fRequestId;
                float time;
                const bool r = requestData(charId, data, time);
                if(!r) continue;
                {
                    ePacket p;
                    p << ePacketType::data;
                    data.write(p);
                    mNet.sendToClient(tcpClientId, p);
                }
            }
        } break;
        case ePacketType::attack: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                eAttackData data;
                data.read(p);
                const int charId = it->second;
                attack(charId, data);
            }
        } break;
        case ePacketType::stopAttack: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                stopAttack(charId);
            }
        } break;
        case ePacketType::respawn: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                eBodyEquipment beq;
                int bodyId;
                const bool r = respawn(charId, beq, bodyId);
                if(r) {
                    ePacket p;
                    p << ePacketType::body;
                    p << bodyId;
                    beq.bodyWrite(p);
                    mNet.sendToClient(tcpClientId, p);
                }
            }
        } break;
        case ePacketType::setSkillId: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                eSkillChoice schoice;
                p >> schoice;
                int skillId;
                p >> skillId;
                setSkillId(charId, schoice, skillId);
            }
        } break;
        case ePacketType::disconnect: {
            handleClientDisconnect(tcpClientId);
        } break;
        case ePacketType::triggerObject: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                int objectId;
                p >> objectId;
                int tx;
                p >> tx;
                int ty;
                p >> ty;
                const auto obj = triggerObject(charId, objectId, tx, ty);

                if(obj) {
                    ePacket p;
                    p << ePacketType::objectStateChanged;
                    p << *obj;
                    mNet.broadcast(p);
                }
            }
        } break;
        case ePacketType::triggerDoors: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                eDoors doors;
                doors.read(p);
                const bool r = triggerDoors(charId, doors);

                if(r) {
                    ePacket p;
                    p << ePacketType::doorsStateChanged;
                    doors.write(p);
                    mNet.broadcast(p);
                }
            }
        } break;
        case ePacketType::dropItem: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                dropItem(charId);
            }
        } break;
        case ePacketType::pickupItem: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                int itemId;
                p >> itemId;
                bool drag;
                p >> drag;
                const bool r = pickupItem(charId, itemId, drag);
                if(r) {
                    eEquipment data;
                    const bool r = receiveEquipment(charId, data);
                    if(!r) continue;
                    ePacket p;
                    p << ePacketType::equipment;
                    data.write(p);
                    mNet.sendToClient(tcpClientId, p);
                } else {
                    ePacket p;
                    p << ePacketType::unblockEquipment;
                    mNet.sendToClient(tcpClientId, p);
                }
            }
        } break;
        case ePacketType::pickupBody: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                uint32_t bodyId;
                p >> bodyId;
                const bool r = pickupBody(charId, bodyId);
                if(r) {
                    {
                        eEquipment data;
                        const bool r = receiveEquipment(charId, data);
                        if(!r) continue;
                        ePacket p;
                        p << ePacketType::equipment;
                        data.write(p);
                        mNet.sendToClient(tcpClientId, p);
                    }
                    {
                        ePacket p;
                        p << ePacketType::bodyPickedUp;
                        p << bodyId;
                        mNet.sendToClient(tcpClientId, p);
                    }
                } else {
                    ePacket p;
                    p << ePacketType::unblockEquipment;
                    mNet.sendToClient(tcpClientId, p);
                }
            }
        } break;
        case ePacketType::rearrangeItems: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                eEquipment eq;
                eq.read(p);
                rearrangeItems(charId, eq);
            }
        } break;
        case ePacketType::consumePotion: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                uint32_t itemId;
                p >> itemId;
                consumePotion(charId, itemId);
            }
        } break;
        case ePacketType::attributes: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                eAttributes attrs;
                attrs.read(p);
                changeAttributes(charId, attrs);
            }
        } break;
        case ePacketType::skills: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                eSkillLevels skillLevels;
                skillLevels.read(p);
                changeSkillLevels(charId, skillLevels);
            }
        } break;
        }
    }
    const auto tcpIds = mNet.removeDisconnectedClients();
    for(const int tcpClientId : tcpIds) {
        handleClientDisconnect(tcpClientId);
    }
}

bool eTcpIpHost::sendMessage(const int clientId,
                             const std::string& text) {
    sendMessageToAll(clientId, text);
    return true;
}

bool eTcpIpHost::triggerDoors(const int clientId,
                              const eDoors& doors) {
    return eLocalServer::triggerDoors(clientId, doors);
}

void eTcpIpHost::sendMessageToAll(
    const int clientId, const std::string& text) {
    ePacket p;
    p << ePacketType::message;
    p << clientId;
    p << text;
    mNet.broadcast(p);

    mMessages.emplace_back(clientId, text);
}

bool eTcpIpHost::handleClientDisconnect(const int tcpClientId) {
    const auto it = mClientIdMap.find(tcpClientId);
    if(it == mClientIdMap.end()) return false;
    const int charId = it->second;
    mClientIdMap.erase(tcpClientId);
    disconnect(charId);
    {
        ePacket p;
        p << ePacketType::userLeft;
        p << charId;
        mNet.broadcast(p);

        mLeftUsers.emplace_back(charId);
    }
    return true;
}

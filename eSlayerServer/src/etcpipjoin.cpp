#include "etcpipjoin.h"

#include "epacketdata.h"

#include <eSlayerHelpers/eattackdata.h>
#include <eSlayerHelpers/eequipment.h>
#include <eSlayerHelpers/escreendimensions.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/edoors.h>
#include <eSlayerHelpers/ebody.h>
#include <eSlayerHelpers/eportals.h>

#include <eSlayerMapGenerator/emap.h>

eTcpIpJoin::~eTcpIpJoin() {
    if(mRunning) {
        mRunning = false;
        mPacketsThread.join();
    }
    if(mInitialized) mNet.shutdown();
}

bool eTcpIpJoin::initialize() {
    mInitialized = mNet.init();
    if(!mInitialized) {
        failed("Disconnected", "Failed to initialize SDL3_net.");
        return false;
    }
    const auto& ip = eServer::ip();;
    const bool r = mNet.connect(ip.data(), 4000);
    if(!r) failed("Disconnected", "Failed to connect to the host.");
    return r;
}

uint32_t eTcpIpJoin::connect() {
    ePacket p;
    p << ePacketType::connect;
    const bool r = mNet.sendToServer(p);
    if(!r) {
        failed("Disconnected", "Failed to send connection request.");
        return 0;
    }
    int32_t clientId = 0;
    const auto handler = [&](ePacket& p, const ePacketType type) {
        if(type == ePacketType::connect) {
            ePacketType type;
            p >> type;
            p >> clientId;
            return true;
        }
        return false;
    };

    mRunning = true;
    mPacketsThread = std::thread([this]() {
        threadWork();
    });

    waitFor(10000, "Connection timed out.", handler);

    return clientId;
}

bool eTcpIpJoin::disconnect(const uint32_t clientId) {
    ePacket p;
    p << ePacketType::disconnect;
    return mNet.sendToServer(p);
}

void eTcpIpJoin::increment(const float by) {
    std::vector<ePacket> packets;
    mPackets.with_lock([&](std::vector<ePacket>& ps) {
        std::swap(ps, packets);
    });
    for(auto& p : packets) {
        handlePacket(p);
    }
}

bool eTcpIpJoin::requestMap(
    const uint32_t clientId,
    const eMoveToMapData& moveData,
    const eMapReadyAction& func) {
    mData = eRequestData();
    mNewData = false;

    ePacket p;
    p << ePacketType::map;
    p << moveData;
    const bool r = mNet.sendToServer(p);
    if(!r) {
        failed("Disconnected", "Failed to send map request to the host.");
        return false;
    }
    const auto handler = [&](ePacket& p, const ePacketType type) {
        if(type == ePacketType::map) {
            ePacketType type;
            p >> type;
            eMapData data;
            data.read(p);
            func(data);
            return true;
        }
        return false;
    };
    return waitFor(10000, "Map request timed out.", handler);
}

bool eTcpIpJoin::spawn(
    const uint32_t clientId,
    eCharacter& c,
    eTeamId& teamId,
    ePointF& spawnPos,
    std::vector<eBody>& bodies,
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
    const auto handler = [&](ePacket& p, const ePacketType type) {
        if(type == ePacketType::spawn) {
            ePacketType type;
            p >> type;
            eSlayers::read(p);
            auto& eq = c.equipment();
            eq.readIds(p);

            eBodies::read(bodies, p);

            eTeams::read(p);
            ePortal::read(p);
            p >> teamId;
            p >> spawnPos;
            return true;
        }
        return false;
    };
    return waitFor(10000, "Character request timed out.", handler);
}

bool eTcpIpJoin::requestData(const uint32_t clientId,
                             eRequestData& data,
                             float& resultTime) {
    {
        ePacket p;
        p << ePacketType::request;
        p << mRequestId++;
        p << serverState();
        const bool r = mNet.sendToServer(p);
        if(!r) failed("Disconnected", "Failed to send a request to the host.");
    }
    if(!mNewData) return false;
    mReceivedId = mData.fRequestId;
    std::swap(mData, data);
    mNewData = false;
    return true;
}

bool eTcpIpJoin::requestEquipment(const uint32_t clientId) {
    ePacket p;
    p << ePacketType::equipment;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a request to the host.");
    return r;
}

bool eTcpIpJoin::receiveEquipment(
    const uint32_t clientId, eEquipment& data) {
    if(!mNewEquipment) return false;
    data = mEquipment;
    mNewEquipment = false;
    return true;
}

bool eTcpIpJoin::unblockEquipment(const uint32_t clientId) {
    const bool unblock = mUnblockEquipment;
    mUnblockEquipment = false;
    return unblock;
}

bool eTcpIpJoin::changeState(
    const uint32_t clientId, const eUnitData& u) {
    ePacket p;
    p << ePacketType::state;
    u.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send state change to the host.");
    return r;
}

bool eTcpIpJoin::attack(const uint32_t clientId,
                        const eAttackData& target) {
    ePacket p;
    p << ePacketType::attack;
    target.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send attack change to the host.");
    return true;
}

bool eTcpIpJoin::stopAttack(const uint32_t clientId) {
    ePacket p;
    p << ePacketType::stopAttack;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send attack change to the host.");
    return true;
}

bool eTcpIpJoin::respawn(const uint32_t clientId,
                         uint32_t& bodyId,
                         ePointF& bodyPos) {
    ePacket p;
    p << ePacketType::respawn;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send respawn request to the host.");
    const auto handler = [&](ePacket& p, const ePacketType type) {
        if(type == ePacketType::body) {
            ePacketType type;
            p >> type;
            p >> bodyId;
            p >> bodyPos;
            return true;
        }
        return false;
    };
    return waitFor(10000, "Body request timed out.", handler);
}

bool eTcpIpJoin::setSkillId(const uint32_t clientId,
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

bool eTcpIpJoin::triggerObject(
    const uint32_t clientId, const eServerObject& obj) {
    ePacket p;
    p << ePacketType::triggerObject;
    p << obj;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send object trigger to the host.");
    return true;
}

bool eTcpIpJoin::triggerDoors(
    const uint32_t clientId, const eServerDoors& doors) {
    ePacket p;
    p << ePacketType::triggerDoors;
    doors.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send trigger doors to the host.");
    return true;
}

bool eTcpIpJoin::pickupItem(const uint32_t clientId,
                            const uint32_t itemId,
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
    const uint32_t clientId) {
    ePacket p;
    p << ePacketType::dropItem;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send item drop to the host.");
    return true;
}

bool eTcpIpJoin::dropGold(
    const uint32_t clientId,
    const uint32_t count) {
    ePacket p;
    p << ePacketType::dropGold;
    p << count;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send gold drop to the host.");
    return true;
}

bool eTcpIpJoin::rearrangeItems(
    const uint32_t clientId, const eEquipment& eq) {
    ePacket p;
    p << ePacketType::rearrangeItems;
    eq.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send equipment rearrangement to the host.");
    return true;
}

bool eTcpIpJoin::changeAttributes(
    const uint32_t clientId, const eAttributes& attrs) {
    ePacket p;
    p << ePacketType::attributes;
    attrs.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send attributes change to the host.");
    return true;
}

bool eTcpIpJoin::changeSkillLevels(
    const uint32_t clientId, const eSkillLevels& skillLevels) {
    ePacket p;
    p << ePacketType::skills;
    skillLevels.write(p);
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send skill levels change to the host.");
    return true;
}

bool eTcpIpJoin::sendMessage(
    const uint32_t clientId, const std::string& text) {
    ePacket p;
    p << ePacketType::message;
    p << text;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a message to the host.");
    return true;
}

bool eTcpIpJoin::consumePotion(
    const uint32_t clientId, const uint32_t itemId) {
    ePacket p;
    p << ePacketType::consumePotion;
    p << itemId;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a potion consumption to the host.");
    return true;
}

bool eTcpIpJoin::pickupBody(
    const uint32_t clientId, const uint32_t bodyId) {
    ePacket p;
    p << ePacketType::pickupBody;
    p << bodyId;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a pickup body to the host.");
    return true;
}

bool eTcpIpJoin::teamAction(
    const uint32_t clientId, const eTeamAction& action) {
    ePacket p;
    p << ePacketType::teamsAction;
    p << action;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a teams action to the host.");
    return true;
}

bool eTcpIpJoin::spawnPortal(const uint32_t clientId) {
    ePacket p;
    p << ePacketType::spawnPortal;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send spawn portal to the host.");
    return true;
}

bool eTcpIpJoin::waitFor(
    const uint32_t wait,
    const std::string& error,
    const ePacketHandler& handler) {
    if(!mRunning) {
        failed("Disconnected", error);
        return false;
    }
    uint32_t time = 0;
    bool found = false;
    while(!found) {
        mPackets.with_lock([&](std::vector<ePacket>& ps) {
            const int iMax = ps.size();
            for(int i = 0; i < iMax; i++) {
                auto& p = ps[i];
                ePacketType type;
                p.peek(type);
                found = handler(p, type);
                if(found) {
                    ps.erase(ps.begin() + i);
                    return;
                }
            }
        });
        if(found) return true;

        SDL_Delay(16);
        time += 16;
        if(time > wait) {
            failed("Disconnected", error);
            return false;
        }
    }
    return true;
}

void eTcpIpJoin::threadWork() {
    while(mRunning) {
        mNet.update();
        eNetPacket pkt;
        while(mNet.pollPacket(pkt)) {
            const auto& p = pkt.fPacket;
            mPackets.with_lock([&](std::vector<ePacket>& v) {
                v.emplace_back(p);
            });
        }
    }
}

void eTcpIpJoin::handlePacket(ePacket& p) {
    ePacketType type;
    p >> type;
    switch(type) {
    case ePacketType::data: {
        const auto state = eServer::serverState();
        const bool r = mData.read(p, state);
        mNewData = mNewData || r;
        eSlayers::readLocations(p);
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
        eSlayer slayer;
        slayer.read(p);
        eSlayers::sSlayers[slayer.fClientId] = slayer;
        mNewUsers.emplace_back(slayer);
    } break;
    case ePacketType::userLeft: {
        uint32_t clientId;
        p >> clientId;
        mLeftUsers.emplace_back(clientId);
    } break;
    case ePacketType::message: {
        uint32_t clientId;
        p >> clientId;
        std::string msg;
        p >> msg;
        mMessages.emplace_back(clientId, msg);
    } break;
    case ePacketType::teams: {
        eTeams::read(p);
    } break;
    case ePacketType::portals: {
        ePortal::read(p);
    } break;
    case ePacketType::objectStateChanged: {
        eServerObject obj;
        p >> obj;
        mObjectStateChanges.emplace_back(obj);
    } break;
    case ePacketType::doorsStateChanged: {
        eServerDoors doors;
        doors.read(p);
        mDoorsStateChanged.emplace_back(doors);
    } break;
    case ePacketType::bodyPickedUp: {
        bool removed;
        p >> removed;
        if(removed) {
            uint32_t bodyId;
            p >> bodyId;
            mBodiesPickedUp.emplace_back(bodyId);
        } else {
            eBody body;
            body.read(p);
            mBodiesChanged.emplace_back(body);
        }
    } break;
    case ePacketType::disconnect: {
        failed("Disconnected", "Host closed the connection.");
    } break;
    default:
        break;
    }
}

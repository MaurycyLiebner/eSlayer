#include "etcpiphost.h"

#include "epacketdata.h"

#include <eSlayerHelpers/eattackdata.h>

eTcpIpHost::~eTcpIpHost() {
    if(mInitialized) mNet.shutdown();
}

bool eTcpIpHost::initialize() {
    eLocalServer::initialize();
    mInitialized = mNet.init();
    if(!mInitialized) {
        failed("Failed to initialize SDL3_net.");
        return false;
    }
    const bool r = mNet.startServer(4000);
    if(!r) failed("Failed to create the host server.");
    return r;
}

void eTcpIpHost::increment(const double by) {
    eLocalServer::increment(by);
    mNet.update();
    eNetPacket pkt;
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
                const int charId = it->second;
                const auto map = requestMap(charId, "town");
                ePacket p;
                p << ePacketType::map;
                map->write(p);
                mNet.sendToClient(tcpClientId, p);
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
        case ePacketType::request: {
            const auto it = mClientIdMap.find(tcpClientId);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                bool r = requestData(charId);
                if(!r) continue;
                eRequestData data;
                p >> data.fRequestId;
                double time;
                r = receiveData(charId, data, time);
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
                respawn(charId);
            }
        } break;
        }
    }
}

#include "etcpiphost.h"

#include "epacketdata.h"

eTcpIpHost::~eTcpIpHost() {
    if(mInitialized) mNet.shutdown();
}

void eTcpIpHost::initialize() {
    eLocalServer::initialize();
    mInitialized = mNet.init();
    if(!mInitialized) return;
    mNet.startServer(4000);
}

void eTcpIpHost::increment(const double by) {
    eLocalServer::increment(by);
    mNet.update();
    eNetPacket pkt;
    while(mNet.pollPacket(pkt)) {
        auto& p = pkt.fPacket;
        ePacketType type;
        p >> type;
        switch(type) {
        case ePacketType::connect: {
            const auto it = mClientIdMap.find(pkt.fClientID);
            int32_t clientId;
            if(it == mClientIdMap.end()) {
                clientId = connect();
                mClientIdMap[pkt.fClientID] = clientId;
            } else {
                clientId = it->second;
            }
            ePacket p;
            p << ePacketType::connect;
            p << clientId;
            mNet.sendToClient(pkt.fClientID, p);
        } break;
        case ePacketType::map: {
            const auto it = mClientIdMap.find(pkt.fClientID);
            if(it != mClientIdMap.end()) {
                const int charId = it->second;
                const auto map = requestMap(charId, "town");
                ePacket p;
                p << ePacketType::map;
                map->write(p);
                mNet.sendToClient(pkt.fClientID, p);
            }
        } break;
        }
    }
}

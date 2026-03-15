#include "etcpipjoin.h"

#include "epacketdata.h"

eTcpIpJoin::eTcpIpJoin(const std::string& ip) :
    mIP(ip) {}

eTcpIpJoin::~eTcpIpJoin() {
    if(mInitialized) mNet.shutdown();
}

void eTcpIpJoin::initialize() {
    mInitialized = mNet.init();
    if(!mInitialized) return;
    mNet.connect(mIP.data(), 4000);
}

int eTcpIpJoin::connect() {
    ePacket p;
    p << ePacketType::connect;
    mNet.sendToServer(p);
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
    }
}

bool eTcpIpJoin::disconnect(const int clientId) {
    return true;
}

void eTcpIpJoin::increment(const double by) {

}

std::shared_ptr<eMap> eTcpIpJoin::requestMap(
    const int clientId, const std::string& name) {
    ePacket p;
    p << ePacketType::map;
    mNet.sendToServer(p);
    while(true) {
        mNet.update();

        eNetPacket pkt;

        while(mNet.pollPacket(pkt)) {
            auto& p = pkt.fPacket;
            ePacketType type;
            p >> type;

            if(type == ePacketType::map) {
                const auto map = std::make_shared<eMap>();
                map->read(p);
                return map;
            }
        }

        SDL_Delay(16);
    }
}

bool eTcpIpJoin::requestUnits(const int clientId) {
    return true;
}

bool eTcpIpJoin::receiveUnits(const int clientId,
                              std::vector<eUnitData>& units,
                              double& resultTime) {
    return true;
}

bool eTcpIpJoin::moveTo(const int clientId,
                        const ePointF& pos) {
    return true;
}

bool eTcpIpJoin::attack(const int clientId,
                        const int targetId) {
    return true;
}

bool eTcpIpJoin::stopAttack(const int clientId) {
    return true;
}

bool eTcpIpJoin::respawn(const int clientId) {
    return true;
}

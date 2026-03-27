#include "etcpipjoin.h"

#include "eSlayerHelpers/eattackdata.h"
#include "epacketdata.h"

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
        if(time > 2000) {
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
            mData = eRequestData();
            mData.read(p);
            mNewData = true;
        } break;
        case ePacketType::weaponData: {
            mWeaponData = eWeaponData();
            mWeaponData.read(p);
            mNewWeaponData = true;
        } break;
        case ePacketType::disconnect: {
            failed("Disconnected", "Host closed the connection.");
        } break;
        default:
            break;
        }
    }
}

std::shared_ptr<eMap> eTcpIpJoin::requestMap(
    const int clientId, const std::string& name) {
    ePacket p;
    p << ePacketType::map;
    const bool r = mNet.sendToServer(p);
    if(!r) {
        failed("Disconnected", "Failed to send map request to the host.");
        return nullptr;
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
                const auto map = std::make_shared<eMap>();
                map->read(p);
                return map;
            }
        }

        SDL_Delay(16);
        time += 16;
        if(time > 2000) {
            failed("Disconnected", "Map request timed out.");
            return nullptr;
        }
    }
}

bool eTcpIpJoin::requestData(const int clientId) {
    ePacket p;
    p << ePacketType::request;
    p << mRequestId++;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a request to the host.");
    return r;
}

bool eTcpIpJoin::requestWeaponData(const int clientId) {
    ePacket p;
    p << ePacketType::weaponData;
    const bool r = mNet.sendToServer(p);
    if(!r) failed("Disconnected", "Failed to send a request to the host.");
    return r;
}

bool eTcpIpJoin::receiveData(const int clientId,
                             eRequestData& data,
                             float& resultTime) {
    if(!mNewData) return false;
    mReceivedId = mData.fRequestId;
    std::swap(mData, data);
    mNewData = false;
    return true;
}

bool eTcpIpJoin::receiveWeaponData(const int clientId,
                                   eWeaponData& data) {
    if(!mNewWeaponData) return false;
    data = mWeaponData;
    mNewWeaponData = false;
    return true;
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

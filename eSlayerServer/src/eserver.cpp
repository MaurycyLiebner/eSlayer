#include "../include/eSlayerServer/eserver.h"

#include "esingleplayerserver.h"
#include "etcpiphost.h"
#include "etcpipjoin.h"

#include <eSlayerHelpers/edoors.h>

#include <eSlayerNet/etcpnetwork.h>

std::shared_ptr<eServer> eSlayerServer::generate(
    const eServerData& data) {
    if(data.fName == "single_player") {
        return std::make_shared<eSinglePlayerServer>(data);
    } else if(data.fName == "tcp_ip_host") {
        return std::make_shared<eTcpIpHost>(data);
    } else if(data.fName == "tcp_ip_join") {
        return std::make_shared<eTcpIpJoin>(data);
    }
    return nullptr;
}

eServer::eServer(const eServerData& data) :
    mData(data) {}

void eServer::setFailureHandler(const eServerFailureHandler& h) {
    mFailure = h;
}

bool eServer::requestMapCall(
    const int clientId,
    const uint8_t id,
    const eMapReadyAction& func) {
    mMapId = id;
    return requestMap(clientId, id, func);
}

std::vector<eOtherUsers> eServer::receiveNewUsers() {
    std::vector<eOtherUsers> result;
    std::swap(mNewUsers, result);
    return result;
}

std::vector<int> eServer::receiveLeftUsers() {
    std::vector<int> result;
    std::swap(mLeftUsers, result);
    return result;
}

std::vector<eMessage> eServer::receiveMessages() {
    std::vector<eMessage> result;
    std::swap(mMessages, result);
    return result;
}

std::vector<eObject> eServer::receiveObjectStateChanges() {
    std::vector<eObject> result;
    std::swap(mObjectStateChanges, result);
    return result;
}

std::vector<eDoors> eServer::receiveDoorsStateChanges() {
    std::vector<eDoors> result;
    std::swap(mDoorsStateChanged, result);
    return result;
}

std::vector<uint32_t> eServer::receiveBodiesPickedUp() {
    std::vector<uint32_t> result;
    std::swap(mBodiesPickedUp, result);
    return result;
}

void eServer::failed(const std::string& msg,
                     const std::string& subMsg) {
    if(mFailure) mFailure(msg, subMsg);
    else eRuntimeThrow(msg + " " + subMsg);
}

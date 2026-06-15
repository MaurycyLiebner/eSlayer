#include "../include/eSlayerServer/eserver.h"

#include "esingleplayerserver.h"
#include "etcpiphost.h"
#include "etcpipjoin.h"

#include <eSlayerHelpers/edoors.h>

#include <eSlayerNet/etcpnetwork.h>

#include <eSlayerMapGenerator/emap.h>

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
    const uint32_t clientId,
    const eMoveToMapData& moveData,
    const eMapReadyAction& func) {
    mMapId = moveData.fMapId;
    return requestMap(clientId, moveData, func);
}

std::vector<eSlayer> eServer::receiveNewUsers() {
    std::vector<eSlayer> result;
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

std::vector<eServerObject> eServer::receiveObjectStateChanges() {
    std::vector<eServerObject> result;
    std::swap(mObjectStateChanges, result);
    return result;
}

std::vector<eServerDoors> eServer::receiveDoorsStateChanges() {
    std::vector<eServerDoors> result;
    std::swap(mDoorsStateChanged, result);
    return result;
}

std::vector<uint32_t> eServer::receiveBodiesPickedUp() {
    std::vector<uint32_t> result;
    std::swap(mBodiesPickedUp, result);
    return result;
}

std::vector<eBody> eServer::receiveBodiesChanged() {
    std::vector<eBody> result;
    std::swap(mBodiesChanged, result);
    return result;
}

void eServer::failed(const std::string& msg,
                     const std::string& subMsg) {
    if(mFailure) mFailure(msg, subMsg);
    else eRuntimeThrow(msg + " " + subMsg);
}

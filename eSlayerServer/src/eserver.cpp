#include "../include/eSlayerServer/eserver.h"

#include "esingleplayerserver.h"
#include "etcpiphost.h"
#include "etcpipjoin.h"

#include <eSlayerHelpers/edoors.h>

#include <eSlayerNet/etcpnetwork.h>

std::shared_ptr<eServer> eSlayerServer::generate(
    const eServerData& data) {
    if(data.fName == "single_player") {
        return std::make_shared<eSinglePlayerServer>();
    } else if(data.fName == "tcp_ip_host") {
        return std::make_shared<eTcpIpHost>();
    } else if(data.fName == "tcp_ip_join") {
        return std::make_shared<eTcpIpJoin>(data.fIp);
    }
    return nullptr;
}

void eServer::setFailureHandler(const eServerFailureHandler& h) {
    mFailure = h;
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

void eServer::failed(const std::string& msg,
                     const std::string& subMsg) {
    if(mFailure) mFailure(msg, subMsg);
    else eRuntimeThrow(msg + " " + subMsg);
}

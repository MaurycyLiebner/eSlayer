#include "../include/eSlayerServer/eserver.h"

#include "esingleplayerserver.h"
#include "etcpiphost.h"
#include "etcpipjoin.h"

#include <eSlayerHelpers/edoors.h>

#include <eSlayerNet/etcpnetwork.h>

#include <eSlayerMapGenerator/emap.h>

uint32_t eServer::sServerState = 0;

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

void eServer::setFailureHandler(
    const eServerFailureHandler& h) {
    mFailure = h;
}

bool eServer::requestMapCall(
    const uint32_t clientId,
    const eMoveToMapData& moveData,
    const eMapReadyAction& func) {
    incServerState();
    return requestMap(clientId, moveData, func);
}

std::vector<eSlayer>
eServer::receiveNewUsers() {
    std::vector<eSlayer> result;
    std::swap(mNewUsers, result);
    return result;
}

std::vector<eSlayer>
eServer::receiveLeftUsers() {
    std::vector<eSlayer> result;
    std::swap(mLeftUsers, result);
    return result;
}

std::vector<eSlayer>
eServer::receiveSlainUsers() {
    std::vector<eSlayer> result;
    std::swap(mSlainUsers, result);
    return result;
}

std::vector<eMessage>
eServer::receiveMessages() {
    std::vector<eMessage> result;
    std::swap(mMessages, result);
    return result;
}

std::vector<eServerObject>
eServer::receiveObjectStateChanges() {
    std::vector<eServerObject> result;
    std::swap(mObjectStateChanges, result);
    return result;
}

std::vector<eServerDoors>
eServer::receiveDoorsStateChanges() {
    std::vector<eServerDoors> result;
    std::swap(mDoorsStateChanged, result);
    return result;
}

std::vector<uint32_t>
eServer::receiveBodiesPickedUp() {
    std::vector<uint32_t> result;
    std::swap(mBodiesPickedUp, result);
    return result;
}

std::vector<eBody>
eServer::receiveBodiesCreated() {
    std::vector<eBody> result;
    std::swap(mBodiesCreated, result);
    return result;
}

std::vector<eBodyItemsTaken>
eServer::receiveBodiesChanged() {
    std::vector<eBodyItemsTaken> result;
    std::swap(mBodyItemsTaken, result);
    return result;
}

std::vector<eEquipmentAction>
eServer::receiveEqActions() {
    std::vector<eEquipmentAction> result;
    std::swap(mEqActions, result);
    return result;
}

std::optional<eSeller>
eServer::receiveSeller() {
    std::optional<eSeller> result;
    std::swap(mSeller, result);
    return result;
}

std::optional<eReplaceItemId>
eServer::receiveReplaceItemId() {
    std::optional<eReplaceItemId> result;
    std::swap(mReplaceItemId, result);
    return result;
}

void eServer::failed(const std::string& msg,
                     const std::string& subMsg) {
    if(mFailure) mFailure(msg, subMsg);
    else eRuntimeThrow(msg + " " + subMsg);
}

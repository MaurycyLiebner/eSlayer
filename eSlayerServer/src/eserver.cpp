#include "../include/eSlayerServer/eserver.h"

#include "esingleplayerserver.h"
#include "etcpiphost.h"
#include "etcpipjoin.h"

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

void eServer::failed(const std::string& msg) {
    if(mFailure) mFailure(msg);
    else eRuntimeThrow(msg);
}

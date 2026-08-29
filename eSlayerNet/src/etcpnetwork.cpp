#include "eSlayerNet/etcpnetwork.h"

#include <eSlayerHelpers/eexceptions.h>

#include <iostream>

bool eTCPNetwork::init() {
    return NET_Init();
}

void eTCPNetwork::shutdown() {
    for(auto& c : mClients) {
        NET_DestroyStreamSocket(c.fSocket);
    }

    mClients.clear();

    if(mClientSocket) {
        NET_DestroyStreamSocket(mClientSocket);
    }

    if(mServer) {
        NET_DestroyServer(mServer);
    }

    NET_Quit();
}

bool eTCPNetwork::startServer(const uint16_t port) {
    mServer = NET_CreateServer(nullptr, port, 0);
    return mServer;
}

bool eTCPNetwork::connect(const char* host, const uint16_t port) {
    const auto addr = NET_ResolveHostname(host);

    if(!addr) return false;

    if(NET_WaitUntilResolved(addr, 5000) != NET_SUCCESS) {
        NET_UnrefAddress(addr);
        return false;
    }

    mClientSocket = NET_CreateClient(addr, port, 0);

    NET_UnrefAddress(addr);

    if(!mClientSocket) return false;

    if(NET_WaitUntilConnected(mClientSocket, 5000) != NET_SUCCESS) {
        mClientSocket = nullptr;
        return false;
    }

    return true;
}

void eTCPNetwork::update() {
    if(mServer) {
        acceptClients();
        receiveFromClients();
    }

    if(mClientSocket) {
        receiveFromServer();
    }
}

void eTCPNetwork::acceptClients() {
    while(true) {
        NET_StreamSocket* sock = nullptr;

        if(!NET_AcceptClient(mServer, &sock)) {
            return;
        }

        if(!sock) return;

        eClient c;
        c.fTcpId = mNextClientID++;
        c.fTimeOut = 0;
        c.fSocket = sock;

        mClients.push_back(c);

        std::cout << "Client connected: " << c.fTcpId << std::endl;
    }
}

std::set<int> eTCPNetwork::removeDisconnectedClients() {
    std::set<int> result;
    for(size_t i = 0; i < mClients.size();) {
        const auto& c = mClients[i];

        const auto status = NET_GetConnectionStatus(c.fSocket);

        if(c.fDisconnected || status != NET_SUCCESS || c.fTimeOut > 200) {
            result.emplace(c.fTcpId);

            NET_DestroyStreamSocket(c.fSocket);
            std::cout << "Client disconnected: " << c.fTcpId << std::endl;
            mClients.erase(mClients.begin() + i);

            continue;
        }

        i++;
    }
    return result;
}

void eTCPNetwork::receiveFromClients() {
    for(auto& c : mClients) {
        if(c.fDisconnected) continue;
        const auto r = receivePackets(
            c.fSocket, c.fTcpId, c.fRecvBuffer);
        switch(r) {
        case eReceiveResult::received: {
            c.fTimeOut = 0;
        } break;
        case eReceiveResult::noData: {
            c.fTimeOut++;
        } break;
        case eReceiveResult::failed: {
            c.fDisconnected = true;
        } break;
        }
    }
}

void eTCPNetwork::receiveFromServer() {
    static std::vector<uint8_t> buffer;
    receivePackets(mClientSocket, 0, buffer);
}

eReceiveResult eTCPNetwork::receivePackets(
    NET_StreamSocket* const sock,
    const int id, std::vector<uint8_t>& buffer) {
    uint8_t temp[65536];

    const int len = NET_ReadFromStreamSocket(sock, temp, sizeof(temp));

    if(len == 0) return eReceiveResult::noData;
    if(len < 0) return eReceiveResult::failed;

    buffer.insert(buffer.end(), temp, temp + len);

    while(true) {
        if(buffer.size() < sizeof(uint32_t)) {
            return eReceiveResult::received;
        }

        uint32_t size;
        memcpy(&size, buffer.data(), sizeof(uint32_t));

        if(buffer.size() < sizeof(uint32_t) + size) {
            return eReceiveResult::received;
        }

        ePacket p;
        p.setData(buffer.data() + sizeof(uint32_t), size);

        {
            std::lock_guard lock(mQueueMutex);
            mPacketQueue.push({id, p});
        }

        buffer.erase(buffer.begin(),
                     buffer.begin() + sizeof(uint32_t) + size);
    }
}

bool eTCPNetwork::sendPacket(NET_StreamSocket* const sock,
                             const ePacket& p) {
    const uint32_t size = (uint32_t)p.size();

    std::vector<uint8_t> buf(sizeof(uint32_t) + size);

    memcpy(buf.data(), &size, sizeof(uint32_t));
    memcpy(buf.data() + sizeof(uint32_t), p.data(), size);

    return NET_WriteToStreamSocket(sock,
            buf.data(), static_cast<int>(buf.size()));
}

bool eTCPNetwork::sendToServer(const ePacket& p) {
    if(!mClientSocket) return false;
    const bool r = sendPacket(mClientSocket, p);
    if(r) return true;
    NET_DestroyStreamSocket(mClientSocket);
    mClientSocket = nullptr;
    return false;
}

bool eTCPNetwork::sendToClient(const int tcpId, const ePacket& p) {
    for(auto& c : mClients) {
        if(c.fTcpId == tcpId) {
            if(c.fDisconnected) return false;
            const bool r = sendPacket(c.fSocket, p);
            if(!r) {
                c.fDisconnected = true;
                return false;
            }
        }
    }
    return true;
}

void eTCPNetwork::broadcast(const ePacket& p) {
    for(int i = 0; i < mClients.size(); i++) {
        auto& c = mClients[i];
        if(c.fDisconnected) continue;

        const bool r = sendPacket(c.fSocket, p);
        if(!r) {
            c.fDisconnected = true;
            i--;
            continue;
        }
    }
}

bool eTCPNetwork::pollPacket(eNetPacket& p) {
    std::lock_guard lock(mQueueMutex);

    if(mPacketQueue.empty()) {
        return false;
    }

    p = mPacketQueue.front();
    mPacketQueue.pop();
    return true;
}

std::vector<std::string> eTCPNetwork::sGetLanIPs() {
    NET_Init();
    int count;
    const auto addresses = NET_GetLocalAddresses(&count);
    std::vector<std::string> result;
    for(int i = 0; i < count; i++) {
        const char *ip = NET_GetAddressString(addresses[i]);
        if(!ip) continue;
        result.emplace_back(ip);
    }
    NET_FreeLocalAddresses(addresses);
    NET_Quit();
    return result;
}

std::string eTCPNetwork::sGetActiveLanIP() {
    const auto ips = sGetLanIPs();
    for(const auto& ip : ips) {
        if (std::strncmp(ip.data(), "192.168.", 8) == 0 ||
            std::strncmp(ip.data(), "10.", 3) == 0 ||
            (std::strncmp(ip.data(), "172.", 4) == 0 &&
             atoi(&ip[4]) >= 16 && atoi(&ip[4]) <= 31)) {
            return ip;
        }
    }
    return "";
}

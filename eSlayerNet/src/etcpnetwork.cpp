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
    mServer = NET_CreateServer(nullptr, port);
    return mServer;
}

bool eTCPNetwork::connect(const char* host, const uint16_t port) {
    const auto addr = NET_ResolveHostname(host);

    if(!addr) return false;

    if(NET_WaitUntilResolved(addr, 5000) != NET_SUCCESS) {
        NET_UnrefAddress(addr);
        return false;
    }

    mClientSocket = NET_CreateClient(addr, port);

    NET_UnrefAddress(addr);

    if(!mClientSocket) return false;

    if(NET_WaitUntilConnected(mClientSocket, 5000) != NET_SUCCESS) {
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
        c.fId = mNextClientID++;
        c.fSocket = sock;

        mClients.push_back(c);

        std::cout << "Client connected: " << c.fId << std::endl;
    }
}

void eTCPNetwork::receiveFromClients() {
    for(auto& c : mClients) {
        receivePackets(c.fSocket, c.fId, c.fRecvBuffer);
    }
}

void eTCPNetwork::receiveFromServer() {
    static std::vector<uint8_t> buffer;
    receivePackets(mClientSocket, 0, buffer);
}

void eTCPNetwork::receivePackets(NET_StreamSocket* const sock,
                                 const int id,
                                 std::vector<uint8_t>& buffer) {
    uint8_t temp[4096];

    int len = NET_ReadFromStreamSocket(sock, temp, sizeof(temp));

    if(len <= 0) return;

    buffer.insert(buffer.end(), temp, temp + len);

    while(true) {
        if(buffer.size() < sizeof(uint32_t)) {
            return;
        }

        uint32_t size;
        memcpy(&size, buffer.data(), sizeof(uint32_t));

        if(buffer.size() < sizeof(uint32_t) + size) {
            return;
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

void eTCPNetwork::sendPacket(NET_StreamSocket* const sock,
                             const ePacket& p) {
    const uint32_t size = (uint32_t)p.size();

    std::vector<uint8_t> buf(sizeof(uint32_t) + size);

    memcpy(buf.data(), &size, sizeof(uint32_t));
    memcpy(buf.data() + sizeof(uint32_t), p.data(), size);

    NET_WriteToStreamSocket(sock, buf.data(), buf.size());
}

void eTCPNetwork::sendToServer(const ePacket& p) {
    if(mClientSocket) {
        sendPacket(mClientSocket, p);
    }
}

void eTCPNetwork::sendToClient(const int id, const ePacket& p) {
    for(auto& c : mClients) {
        if(c.fId == id) {
            sendPacket(c.fSocket, p);
        }
    }
}

void eTCPNetwork::broadcast(const ePacket& p) {
    for(auto& c : mClients) {
        sendPacket(c.fSocket, p);
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
    NET_Address **addresses = NET_GetLocalAddresses(&count);
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

#ifndef ETCPIPHOST_H
#define ETCPIPHOST_H

#include "elocalserver.h"

#include <eSlayerNet/etcpnetwork.h>

class eTcpIpHost : public eLocalServer {
public:
    ~eTcpIpHost();

    bool initialize() override;

    void increment(const float by) override;

    bool sendMessage(const int clientId,
                     const std::string& text) override;
private:
    void sendMessageToAll(const int clientId, const std::string& text);
    bool handleClientDisconnect(const int tcpClientId);

    eTCPNetwork mNet;
    bool mInitialized = false;
    // maps eTCPNetwork client id to charId
    std::map<int, int> mClientIdMap;
};

#endif // ETCPIPHOST_H

#ifndef ETCPIPHOST_H
#define ETCPIPHOST_H

#include "elocalserver.h"

#include <eSlayerNet/etcpnetwork.h>

class eTcpIpHost : public eLocalServer {
public:
    ~eTcpIpHost();

    bool initialize() override;

    void increment(const double by) override;
private:
    eTCPNetwork mNet;
    bool mInitialized = false;
    // maps eTCPNetwork client id to charId
    std::map<int, int> mClientIdMap;
};

#endif // ETCPIPHOST_H

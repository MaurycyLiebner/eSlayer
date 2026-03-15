#ifndef ETCPIPHOST_H
#define ETCPIPHOST_H

#include "elocalserver.h"

#include <eSlayerNet/etcpnetwork.h>

class eTcpIpHost : public eLocalServer {
public:
    ~eTcpIpHost();

    void initialize() override;

    void increment(const double by) override;
private:
    eTCPNetwork mNet;
    bool mInitialized = false;
    // maps charId to eTCPNetwork client id
    std::map<int, int> mClientIdMap;
};

#endif // ETCPIPHOST_H

#ifndef ETCPIPJOIN_H
#define ETCPIPJOIN_H

#include "eSlayerServer/eserver.h"

#include <eSlayerNet/etcpnetwork.h>
#include <eSlayerHelpers/erequestdata.h>

class eTcpIpJoin : public eServer {
public:
    eTcpIpJoin(const std::string& ip);
    ~eTcpIpJoin();

    bool initialize() override;

    int connect() override;
    bool disconnect(const int clientId) override;

    void increment(const double by) override;

    std::shared_ptr<eMap>
    requestMap(const int clientId,
               const std::string& name) override;

    bool requestData(const int clientId) override;

    bool receiveData(const int clientId,
                     eRequestData& data,
                     double& resultTime) override;

    bool changeState(const int clientId,
                     const eUnitData& u) override;

    bool attack(const int clientId,
                const eAttackData& target) override;
    bool stopAttack(const int clientId) override;

    bool respawn(const int clientId) override;
private:
    const std::string mIP;
    eTCPNetwork mNet;
    bool mInitialized = false;
    eRequestData mData;
    bool mNewData = false;
    uint32_t mRequestId = 0;
    uint32_t mReceivedId = 0;
};

#endif // ETCPIPJOIN_H

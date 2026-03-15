#ifndef ETCPIPJOIN_H
#define ETCPIPJOIN_H

#include "eSlayerServer/eserver.h"

#include <eSlayerNet/etcpnetwork.h>

class eTcpIpJoin : public eServer {
public:
    eTcpIpJoin(const std::string& ip);
    ~eTcpIpJoin();

    void initialize() override;

    int connect() override;
    bool disconnect(const int clientId) override;

    void increment(const double by) override;

    std::shared_ptr<eMap>
    requestMap(const int clientId,
               const std::string& name) override;

    bool requestUnits(const int clientId) override;

    bool receiveUnits(const int clientId,
                      std::vector<eUnitData>& units,
                      double& resultTime) override;

    bool moveTo(const int clientId,
                const ePointF& pos) override;

    bool attack(const int clientId,
                const int targetId) override;
    bool stopAttack(const int clientId) override;

    bool respawn(const int clientId) override;
private:
    const std::string mIP;
    eTCPNetwork mNet;
    bool mInitialized = false;
};

#endif // ETCPIPJOIN_H

#ifndef ESINGLEPLAYERSERVER_H
#define ESINGLEPLAYERSERVER_H

#include "eSlayerServer/eserver.h"

#include "eserverclienthandler.h"
#include "eserverarea.h"

#include <map>

class eSinglePlayerServer : public eServer {
public:
    int connect() override;
    bool disconnect(const int clientId) override;

    void increment(const double by) override;

    std::shared_ptr<eMap>
    requestMap(const int clientId,
               const std::string& name) override;

    bool requestUnits(const int clientId) override;

    int receiveUnits(const int clientId,
                     std::vector<eUnitData>& units,
                     double& resultTime) override;

    bool moveTo(const int clientId,
                const ePointF& pos) override;
public:
    eServerClientHandler* clientHandler(const int clientId);

    double mTime = 0.;

    std::vector<std::shared_ptr<eServerArea>> mAreas;
    std::map<std::string, std::shared_ptr<eMap>> mMaps;
    std::map<int, std::shared_ptr<eServerClientHandler>> mClientHandlers;
};

#endif // ESINGLEPLAYERSERVER_H

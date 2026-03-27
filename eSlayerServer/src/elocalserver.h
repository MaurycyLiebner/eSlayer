#ifndef ELOCALSERVER_H
#define ELOCALSERVER_H

#include "eSlayerServer/eserver.h"

#include "eserverclienthandler.h"
#include "eserverarea.h"

#include <map>

struct eAttackData;

class eLocalServer : public eServer {
public:
    bool initialize() override;

    int connect() override;
    bool disconnect(const int clientId) override;

    void increment(const float by) override;

    std::shared_ptr<eMap>
    requestMap(const int clientId,
               const std::string& name) override;

    bool requestData(const int clientId) override;

    bool receiveData(const int clientId,
                     eRequestData& data,
                     float& resultTime) override;

    bool changeState(const int clientId,
                     const eUnitData& u) override;

    bool attack(const int clientId,
                const eAttackData& target) override;
    bool stopAttack(const int clientId) override;

    bool respawn(const int clientId) override;

    bool setSkillId(const int clientId,
                    const eSkillChoice schoice,
                    const int skillId) override;
public:
    eServerClientHandler* clientHandler(const int clientId);

    float mTime = 0.f;

    std::map<std::string, std::shared_ptr<eServerArea>> mAreas;
    std::map<std::string, std::shared_ptr<eMap>> mMaps;
    std::map<int, std::shared_ptr<eServerClientHandler>> mClientHandlers;
};

#endif // ELOCALSERVER_H

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

    std::shared_ptr<eMap> requestMap(
        const int clientId, const std::string& name) override;
    bool spawn(const int clientId,
               const eEquipment& eq,
               const eScreenDimensions& screenDims) override;

    bool requestData(const int clientId,
                     eRequestData& data,
                     float& resultTime) override;
    bool requestWeaponData(const int clientId) override;
    bool requestEquipment(const int clientId) override;
    bool receiveWeaponData(const int clientId,
                           eWeaponData& data) override;
    bool receiveEquipment(const int clientId,
                          eEquipment& data) override;

    bool changeState(const int clientId,
                     const eUnitData& u) override;

    bool attack(const int clientId,
                const eAttackData& target) override;
    bool stopAttack(const int clientId) override;

    bool respawn(const int clientId) override;

    bool setSkillId(const int clientId,
                    const eSkillChoice schoice,
                    const int skillId) override;

    bool pickupItem(const int clientId,
                    const int itemId,
                    const bool drag) override;
    bool dropItem(const int clientId,
                  const int itemId) override;
    bool rearrangeItems(const int clientId,
                        const eEquipment& eq) override;
private:
    eServerClientHandler* clientHandler(const int clientId);

    float mTime = 0.f;

    std::map<std::string, std::shared_ptr<eServerArea>> mAreas;
    std::map<std::string, std::shared_ptr<eMap>> mMaps;
    std::map<int, std::shared_ptr<eServerClientHandler>> mClientHandlers;
};

#endif // ELOCALSERVER_H

#ifndef ETCPIPJOIN_H
#define ETCPIPJOIN_H

#include "eSlayerServer/eserver.h"

#include <eSlayerNet/etcpnetwork.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eequipment.h>

class eTcpIpJoin : public eServer {
public:
    eTcpIpJoin(const std::string& ip);
    ~eTcpIpJoin();

    bool initialize() override;

    int connect() override;
    bool disconnect(const int clientId) override;

    void increment(const float by) override;

    std::shared_ptr<eMap>
    requestMap(const int clientId,
               const std::string& name,
               const eEquipment& eq) override;

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
    const std::string mIP;
    eTCPNetwork mNet;
    bool mInitialized = false;
    eRequestData mData;
    bool mNewData = false;
    eWeaponData mWeaponData;
    bool mNewWeaponData = false;
    eEquipment mEquipment;
    bool mNewEquipment = false;
    uint32_t mRequestId = 0;
    uint32_t mReceivedId = 0;
};

#endif // ETCPIPJOIN_H

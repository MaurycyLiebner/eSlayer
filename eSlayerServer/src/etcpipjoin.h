#ifndef ETCPIPJOIN_H
#define ETCPIPJOIN_H

#include "eSlayerServer/eserver.h"

#include <eSlayerNet/etcpnetwork.h>
#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eequipment.h>

class eTcpIpJoin : public eServer {
public:
    using eServer::eServer;
    ~eTcpIpJoin();

    bool initialize() override;

    int connect() override;
    bool disconnect(const int clientId) override;

    void increment(const float by) override;

    bool requestMap(const int clientId,
                    const std::string& name,
                    eMapData& data) override;
    bool spawn(const int clientId,
               eCharacter& c,
               eTeamId& teamId,
               const eScreenDimensions& screenDims) override;

    bool requestData(const int clientId,
                     eRequestData& data,
                     float& resultTime) override;
    bool requestEquipment(const int clientId) override;
    bool receiveEquipment(const int clientId,
                          eEquipment& data) override;
    bool unblockEquipment(const int clientId) override;

    bool changeState(const int clientId,
                     const eUnitData& u) override;

    bool attack(const int clientId,
                const eAttackData& target) override;
    bool stopAttack(const int clientId) override;

    bool respawn(const int clientId) override;

    bool setSkillId(const int clientId,
                    const eSkillChoice schoice,
                    const int skillId) override;

    std::shared_ptr<eObject> triggerObject(
        const int clientId, const int objectId,
        const int tx, const int ty) override;

    bool triggerDoors(const int clientId,
                      const eDoors& doors) override;

    bool pickupItem(const int clientId,
                    const int itemId,
                    const bool drag) override;
    bool dropItem(const int clientId) override;
    bool rearrangeItems(const int clientId,
                        const eEquipment& eq) override;
    bool changeAttributes(const int clientId,
                          const eAttributes& attrs) override;
    bool changeSkillLevels(
        const int clientId,
        const eSkillLevels& skillLevels) override;
    bool sendMessage(const int clientId,
                     const std::string& text) override;
    bool consumePotion(const int clientId,
                       const uint32_t itemId) override;
    bool pickupBody(const int clientId,
                    const int32_t bodyId) override;
private:
    eTCPNetwork mNet;
    bool mInitialized = false;

    eRequestData mData;
    bool mNewData = false;

    eEquipment mEquipment;
    bool mNewEquipment = false;

    bool mUnblockEquipment = false;

    uint32_t mRequestId = 0;
    uint32_t mReceivedId = 0;
};

#endif // ETCPIPJOIN_H

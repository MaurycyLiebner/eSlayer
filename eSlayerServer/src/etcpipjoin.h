#ifndef ETCPIPJOIN_H
#define ETCPIPJOIN_H

#include "eSlayerServer/eserver.h"
#include "ethreadsafe.h"

#include <eSlayerNet/etcpnetwork.h>

#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eequipment.h>

#include <thread>

enum class ePacketType : uint8_t;

class eTcpIpJoin : public eServer {
public:
    using eServer::eServer;
    ~eTcpIpJoin();

    bool initialize() override;

    uint32_t connect() override;
    bool disconnect(const uint32_t clientId) override;

    void increment(const float by) override;

    bool requestMap(const uint32_t clientId,
                    const eMoveToMapData& moveData,
                    const eMapReadyAction& func) override;
    bool spawn(const uint32_t clientId,
               eCharacter& c,
               eTeamId& teamId,
               ePointF& spawnPos,
               std::vector<eBody>& bodies,
               const eScreenDimensions& screenDims) override;

    bool requestData(const uint32_t clientId,
                     eRequestData& data,
                     float& resultTime) override;
    bool requestEquipment(const uint32_t clientId) override;
    bool receiveEquipment(const uint32_t clientId,
                          eEquipment& data) override;
    bool unblockEquipment(const uint32_t clientId) override;

    std::optional<eSlayerQuests>
    receiveQuests(const uint32_t clientId) override;

    bool
    heardTalk(const uint32_t clientId,
              const eConvoId& talk) override;

    bool changeState(const uint32_t clientId,
                     const eUnitData& u) override;

    bool attack(const uint32_t clientId,
                const eAttackData& target) override;
    bool stopAttack(const uint32_t clientId) override;

    bool createBody(const uint32_t clientId) override;

    bool setSkillId(const uint32_t clientId,
                    const eSkillChoice schoice,
                    const int skillId) override;

    bool triggerObject(const uint32_t clientId,
                       const eServerObject& obj) override;

    bool triggerDoors(const uint32_t clientId,
                      const eServerDoors& doors) override;

    bool pickupItem(const uint32_t clientId,
                    const uint32_t itemId,
                    const bool drag) override;
    bool dropItem(const uint32_t clientId) override;
    bool dropGold(const uint32_t clientId,
                  const uint32_t count) override;
    bool rearrangeItems(const uint32_t clientId,
                        const eEquipment& eq) override;
    bool changeAttributes(const uint32_t clientId,
                          const eAttributes& attrs) override;
    bool changeSkillLevels(
        const uint32_t clientId,
        const eSkillLevels& skillLevels) override;
    bool sendMessage(const uint32_t clientId,
                     const std::string& text) override;
    bool consumePotion(const uint32_t clientId,
                       const uint32_t itemId) override;
    bool pickupBody(const uint32_t clientId,
                    const uint32_t bodyId) override;
    bool teamAction(const uint32_t clientId,
                    const eTeamAction& action) override;

    bool spawnPortal(const uint32_t clientId) override;

    bool equipmentAction(const uint32_t clientId,
                         const eEquipmentAction& a) override;
    bool buyAction(const uint32_t clientId,
                   const eBuyAction& a) override;
    bool sellAction(const uint32_t clientId,
                    const eSellAction& a) override;
    bool requestSeller(const uint32_t clientId,
                       const uint32_t sellerId) override;
private:
    using ePacketHandler = std::function<bool(
        ePacket& p, const ePacketType type)>;
    bool waitFor(const uint32_t wait,
                 const std::string& error,
                 const ePacketHandler& handler);
    void threadWork();
    void handlePacket(ePacket& p);

    std::thread mPacketsThread;
    bool mRunning = false;
    eThreadSafe<std::vector<ePacket>> mPackets;

    eTCPNetwork mNet;
    bool mInitialized = false;

    std::optional<eSlayerQuests> mQuests;

    eRequestData mData;
    bool mNewData = false;

    eEquipment mEquipment;
    bool mNewEquipment = false;

    bool mUnblockEquipment = false;

    uint32_t mRequestId = 0;
    uint32_t mReceivedId = 0;
};

#endif // ETCPIPJOIN_H

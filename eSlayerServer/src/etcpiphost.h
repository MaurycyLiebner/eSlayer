#ifndef ETCPIPHOST_H
#define ETCPIPHOST_H

#include "elocalserver.h"

#include <eSlayerNet/etcpnetwork.h>

#include <thread>

class eTcpIpHost : public eLocalServer {
public:
    using eLocalServer::eLocalServer;
    ~eTcpIpHost();

    bool initialize() override;

    void increment(const float by) override;

    bool sendMessage(const uint32_t clientId,
                     const std::string& text) override;

    bool requestMap(const uint32_t clientId,
                    const eMoveToMapData& moveData,
                    const eMapReadyAction& func) override;

    bool spawn(const uint32_t clientId,
               eCharacter& c,
               eTeamId& teamId,
               ePointF& spawnPos,
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
    bool rearrangeItems(const uint32_t clientId,
                        const eEquipment& eq) override;
    bool changeAttributes(const uint32_t clientId,
                          const eAttributes& attrs) override;
    bool changeSkillLevels(const uint32_t clientId,
                           const eSkillLevels& skillLevels) override;
    bool consumePotion(const uint32_t clientId,
                       const uint32_t itemId) override;
    bool pickupBody(const uint32_t clientId, const uint32_t bodyId) override;

    void checkMapsReady() override;

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
    void sendMessageToAll(const uint32_t clientId, const std::string& text);
    bool handleClientDisconnect(const int tcpClientId);
    void threadWork(const float fpsClamp, const float by);
    void processPacket(eNetPacket& pkt);

    bool triggerDoorsAndSend(
        const uint32_t clientId, const eServerDoors& doors);
    bool triggerObjectAndSend(const uint32_t clientId,
                              const eServerObject& obj);

    void sendToMapClients(const uint8_t mapId, const ePacket& p);

    bool synchronizeEq(const uint32_t clientId,
                       const int tcpClientId);

    std::thread mThread;
    bool mRunning = false;
    mutable std::shared_mutex mMutex;

    eTCPNetwork mNet;
    bool mInitialized = false;
    // maps eTCPNetwork client id to charId
    std::map<int, uint32_t> mClientIdMap;
    uint16_t mTeamsVersion = 0;
    uint16_t mPortalsVersion = 0;
};

#endif // ETCPIPHOST_H

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

    bool sendMessage(const int clientId,
                     const std::string& text) override;

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

    bool respawn(const int clientId,
                 eBodyEquipment& beq,
                 int& bodyId) override;

    bool setSkillId(const int clientId,
                    const eSkillChoice schoice,
                    const int skillId) override;

    std::shared_ptr<eObject>
    triggerObject(const int clientId,
                  const int objectId,
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
    bool changeSkillLevels(const int clientId,
                           const eSkillLevels& skillLevels) override;
    bool consumePotion(const int clientId,
                       const uint32_t itemId) override;
    bool pickupBody(const int clientId, const uint32_t bodyId) override;

    void checkMapsReady() override;
private:
    void sendMessageToAll(const int clientId, const std::string& text);
    bool handleClientDisconnect(const int tcpClientId);
    void threadWork(const float fpsClamp, const float by);
    void processPacket(eNetPacket& pkt);

    bool triggerDoorsAndSend(const int charId, const eDoors& doors);
    std::shared_ptr<eObject>
    triggerObjectAndSend(const int charId, const int objectId,
                         const int tx, const int ty);

    std::thread mThread;
    bool mRunning = false;
    mutable std::shared_mutex mMutex;

    eTCPNetwork mNet;
    bool mInitialized = false;
    // maps eTCPNetwork client id to charId
    std::map<int, int> mClientIdMap;
    int mTeamsVersion = 0;
};

#endif // ETCPIPHOST_H

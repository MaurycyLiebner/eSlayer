#ifndef ELOCALSERVER_H
#define ELOCALSERVER_H

#include "eSlayerServer/eserver.h"

#include "eserverclienthandler.h"
#include "eserverarea.h"
#include "ethreadsafe.h"

#include <map>

struct eAttackData;

struct eMapAndArea {
    uint8_t fId;
    std::shared_ptr<eMap> fMap;
    std::shared_ptr<eServerArea> fArea;
};

class eLocalServer : public eServer {
public:
    using eServer::eServer;
    bool initialize() override;

    int connect() override;
    bool disconnect(const int clientId) override;

    void increment(const float by) override;

    bool requestMap(const int clientId,
                    const uint8_t mapId,
                    const eMapReadyAction& func) override;
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

    bool triggerObject(const int clientId,
                       eServerObject& obj) override;

    bool triggerDoors(const int clientId,
                      const eServerDoors& doors) override;

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
protected:
    bool changeTeam(const int clientId,
                    const eTeamId newTeam);
    int clientMapId(const int clientId);
    eServerClientHandler* clientHandler(const int clientId);
    std::map<int, std::shared_ptr<eServerClientHandler>> mClientHandlers;
private:
    void mapReady(const eMapAndArea& ma);

    std::map<uint8_t, eMapAndArea> mMaps;
    using eOMapReadyAction = std::function<void(const eMapAndArea& ma)>;
    std::map<uint8_t, std::vector<eOMapReadyAction>> mMapReadyActions;

    eThreadSafe<std::vector<eMapAndArea>> mReady;
};

#endif // ELOCALSERVER_H

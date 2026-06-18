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

    bool changeState(const uint32_t clientId,
                     const eUnitData& u) override;

    bool attack(const uint32_t clientId,
                const eAttackData& target) override;
    bool stopAttack(const uint32_t clientId) override;

    bool respawn(const uint32_t clientId,
                 uint32_t& bodyId,
                 ePointF& bodyPos) override;

    bool setSkillId(const uint32_t clientId,
                    const eSkillChoice schoice,
                    const int skillId) override;

    bool triggerObject(const uint32_t clientId,
                       const eServerObject& obj) override;
    bool triggerObjectImpl(const uint32_t clientId,
                           eServerObject& obj);

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
    bool changeSkillLevels(const uint32_t clientId,
                           const eSkillLevels& skillLevels) override;
    bool consumePotion(const uint32_t clientId,
                       const uint32_t itemId) override;
    bool pickupBody(const uint32_t clientId,
                    const uint32_t bodyId) override;

    bool pickupBody(const uint32_t clientId,
                    const uint32_t bodyId,
                    bool& bodyRemoved,
                    eBody& body);

    void checkMapsReady() override;

    bool teamAction(const uint32_t clientId,
                    const eTeamAction& action) override;

    bool spawnPortal(const uint32_t clientId) override;
protected:
    bool changeTeam(const uint32_t clientId,
                    const eTeamId newTeam);
    int clientMapId(const uint32_t clientId);
    eServerClientHandler* clientHandler(const uint32_t clientId);
    std::map<int, std::shared_ptr<eServerClientHandler>> mClientHandlers;
private:
    void mapReady(const eMapAndArea& ma);
    using eMapReadyBaseAction = std::function<void(const eMapAndArea& ma)>;
    bool requestMap(const uint8_t mapId,
                    const eMapReadyBaseAction& func);

    std::map<uint8_t, eMapAndArea> mMaps;
    using eOMapReadyAction = std::function<void(const eMapAndArea& ma)>;
    std::map<uint8_t, std::vector<eOMapReadyAction>> mMapReadyActions;

    eThreadSafe<std::vector<eMapAndArea>> mReady;
};

#endif // ELOCALSERVER_H

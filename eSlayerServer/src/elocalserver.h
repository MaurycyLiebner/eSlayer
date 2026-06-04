#ifndef ELOCALSERVER_H
#define ELOCALSERVER_H

#include "eSlayerServer/eserver.h"

#include "eserverclienthandler.h"
#include "eserverarea.h"
#include "ethreadsafevector.h"

#include <map>

struct eAttackData;

struct eMapAndArea {
    std::string fName;
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
                    const std::string& name,
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

    bool respawn(const int clientId) override;

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
    bool pickupBody(const int clientId,
                    const int32_t bodyId) override;

    void mapReady(const eMapAndArea& ma);
    void checkMapsReady() override;
protected:
    eServerClientHandler* clientHandler(const int clientId);
    std::map<int, std::shared_ptr<eServerClientHandler>> mClientHandlers;
private:
    float mTime = 0.f;

    std::map<std::string, eMapAndArea> mMaps;
    using eOMapReadyAction = std::function<void(const eMapAndArea& ma)>;
    std::map<std::string, std::vector<eOMapReadyAction>> mMapReadyActions;

    eThreadSafeVector<eMapAndArea> mReady;
};

#endif // ELOCALSERVER_H

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

    bool requestMap(const int clientId,
                    const std::string& name,
                    eMapData& data) override;
    bool spawn(const int clientId,
               eCharacter& c,
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

    bool pickupItem(const int clientId,
                    const int itemId,
                    const bool drag) override;
    bool dropItem(const int clientId,
                  const int itemId) override;
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
protected:
    eServerClientHandler* clientHandler(const int clientId);
    std::map<int, std::shared_ptr<eServerClientHandler>> mClientHandlers;
private:
    float mTime = 0.f;

    std::map<std::string, std::shared_ptr<eServerArea>> mAreas;
    std::map<std::string, std::shared_ptr<eMap>> mMaps;
};

#endif // ELOCALSERVER_H

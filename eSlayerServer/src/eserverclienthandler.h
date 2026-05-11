#ifndef ESERVERCLIENTHANDLER_H
#define ESERVERCLIENTHANDLER_H

#include "eserverarea.h"

#include <eSlayerHelpers/erequestdata.h>

#include <memory>

struct eAttackData;
struct eDoors;

struct eDataRequest {
    float fTime;
    eRequestData fData;
};

class eServerClientHandler {
public:
    eServerClientHandler(const int clientId);

    bool requestEquipment();
    bool receiveData(eRequestData& data,
                     float& resultTime);
    bool receiveEquipment(eEquipment& data);

    void setArea(const std::shared_ptr<eServerArea>& a) { mArea = a; }

    bool changeState(const eUnitData& u);
    bool attack(const eAttackData& target);
    bool stopAttack();
    bool respawn();
    bool spawn(eCharacter& c,
               eTeamId& teamId,
               const eScreenDimensions& screenDims);
    bool disconnect();
    bool setSkillId(const eSkillChoice schoice,
                    const int skillId);

    std::shared_ptr<eObject> triggerObject(
        const int objectId,
        const int tx, const int ty);

    bool triggerDoors(const eDoors& doors);

    bool pickupItem(const int itemId, const bool drag);
    bool dropItem();
    bool rearrangeItems(const eEquipment& eq);
    bool changeAttributes(const eAttributes& attrs);
    bool changeSkillLevels(const eSkillLevels& skillLevels);
    bool consumePotion(const uint32_t itemId);
    bool pickupBody(const uint32_t bodyId);

    const std::string& name() const { return mName; }
    void setName(const std::string& name) { mName = name; }
private:
    const int mClientId;
    std::string mName;
    std::shared_ptr<eServerArea> mArea;
    std::vector<eDataRequest> mDataRequests;
};

#endif // ESERVERCLIENTHANDLER_H

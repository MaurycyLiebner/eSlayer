#ifndef ESERVERCLIENTHANDLER_H
#define ESERVERCLIENTHANDLER_H

#include "eserverarea.h"

#include <eSlayerHelpers/erequestdata.h>
#include <eSlayerHelpers/eequipmentaction.h>

#include <memory>

struct eAttackData;
struct eDoors;

struct eDataRequest {
    float fTime;
    eRequestData fData;
};

class eServerClientHandler {
public:
    eServerClientHandler(const uint32_t clientId);

    bool requestEquipment();
    bool receiveData(eRequestData& data,
                     float& resultTime);
    bool receiveEquipment(eEquipment& data);

    const std::shared_ptr<eServerArea>& area() const { return mArea; }
    void setArea(const std::shared_ptr<eServerArea>& a) { mArea = a; }

    std::shared_ptr<eMap> map() const;

    bool changeState(const eUnitData& u);
    bool attack(const eAttackData& target);
    bool stopAttack();
    bool respawn(uint32_t& bodyId, ePointF& bodyPos);
    bool spawn(eCharacter& c,
               eTeamId& teamId,
               ePointF& spawnPos,
               std::vector<eBody>& bodies,
               const eScreenDimensions& screenDims);
    bool disconnect();
    bool setSkillId(const eSkillChoice schoice,
                    const int skillId);

    bool triggerObject(eServerObject& obj);
    bool triggerDoors(const eServerDoors& doors);

    bool pickupItem(const uint32_t itemId, const bool drag,
                    eEquipmentAction& action);
    bool dropItem();
    bool dropGold(const uint32_t count);
    bool rearrangeItems(const eEquipment& eq);
    bool changeAttributes(const eAttributes& attrs);
    bool changeSkillLevels(const eSkillLevels& skillLevels);
    bool consumePotion(const uint32_t itemId);
    bool pickupBody(const uint32_t bodyId,
                    bool& bodyRemoved,
                    eBodyItemsTaken& taken);
    bool changeTeam(const eTeamId newTeam);
    bool spawnPortal(uint32_t& portalId,
                     uint8_t& mapId,
                     uint8_t& areaId,
                     ePointF& pos);
    bool equipmentAction(const eEquipmentAction& a);
    bool buyAction(const eBuyAction& a, uint32_t& newItemId);
private:
    const uint32_t mClientId;
    std::shared_ptr<eServerArea> mArea;
};

#endif // ESERVERCLIENTHANDLER_H

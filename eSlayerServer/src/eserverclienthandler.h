#ifndef ESERVERCLIENTHANDLER_H
#define ESERVERCLIENTHANDLER_H

#include "eserverarea.h"

#include <eSlayerHelpers/erequestdata.h>

#include <memory>

struct eAttackData;

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
    bool spawn(eCharacter& c, const eScreenDimensions& screenDims);
    bool disconnect();
    bool setSkillId(const eSkillChoice schoice,
                    const int skillId);

    bool pickupItem(const int itemId, const bool drag);
    bool dropItem(const int itemId);
    bool rearrangeItems(const eEquipment& eq);
    bool changeAttributes(const eAttributes& attrs);
    bool changeSkillLevels(const eSkillLevels& skillLevels);
    bool consumePotion(const uint32_t itemId);

    const std::string& name() const { return mName; }
    void setName(const std::string& name) { mName = name; }
private:
    const int mClientId;
    std::string mName;
    std::shared_ptr<eServerArea> mArea;
    std::vector<eDataRequest> mDataRequests;
};

#endif // ESERVERCLIENTHANDLER_H

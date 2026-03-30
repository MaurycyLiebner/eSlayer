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
    bool spawn(const eEquipment& eq,
               const eScreenDimensions& screenDims);
    bool disconnect();
    bool setSkillId(const eSkillChoice schoice,
                    const int skillId);

    bool pickupItem(const int itemId, const bool drag);
    bool dropItem(const int itemId);
    bool rearrangeItems(const eEquipment& eq);
private:
    const int mClientId;
    std::shared_ptr<eServerArea> mArea;
    std::vector<eDataRequest> mDataRequests;
};

#endif // ESERVERCLIENTHANDLER_H

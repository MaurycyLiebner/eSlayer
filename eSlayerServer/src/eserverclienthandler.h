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

    bool requestData();
    bool requestWeaponData();
    bool receiveData(eRequestData& data,
                     float& resultTime);
    bool receiveWeaponData(eWeaponData& data);

    void setArea(const std::shared_ptr<eServerArea>& a) { mArea = a; }

    bool changeState(const eUnitData& u);
    bool attack(const eAttackData& target);
    bool stopAttack();
    bool respawn();
    bool spawn();
    bool disconnect();
    bool setSkillId(const eSkillChoice schoice,
                    const int skillId);
private:
    const int mClientId;
    std::shared_ptr<eServerArea> mArea;
    std::vector<eDataRequest> mDataRequests;
};

#endif // ESERVERCLIENTHANDLER_H

#ifndef ESERVER_H
#define ESERVER_H

#include "eslayerserverexport.h"

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerHelpers/eskillchoice.h>
#include <eSlayerHelpers/emapportion.h>

#include <memory>

class eRequestData;
class eCharacter;
struct eAttackData;
struct eWeaponData;
struct eEquipment;
struct eAttributes;
struct eSkillLevels;
struct eScreenDimensions;

struct eServerData {
    std::string fName;
    std::string fIp;
};

struct eOtherUsers {
    int fClientId;
    std::string fName;
    bool fJustJoined;

    eOtherUsers(const int id,
                const std::string& name,
                const bool joined) :
        fClientId(id), fName(name),
        fJustJoined(joined) {}
};

struct eMessage {
    int fClientId;
    std::string fMsg;

    eMessage(const int id,
             const std::string& msg) :
        fClientId(id), fMsg(msg) {}
};

using eServerFailureHandler = std::function<void(
    const std::string& msg, const std::string& subMsg)>;

class ESLAYERSERVER_API eServer {
public:
    void setFailureHandler(const eServerFailureHandler& h);

    virtual bool initialize() = 0;

    virtual int connect() = 0;
    virtual bool disconnect(const int clientId) = 0;

    virtual void increment(const float by) = 0;

    virtual bool
    requestMap(const int clientId,
               const std::string& name,
               eMapData& data) = 0;
    virtual bool
    spawn(const int clientId,
          eCharacter& c,
          eTeamId& teamId,
          const eScreenDimensions& screenDims) = 0;

    virtual bool
    requestData(const int clientId,
                eRequestData& data,
                float& resultTime) = 0;
    virtual bool
    requestEquipment(const int clientId) = 0;
    virtual bool
    receiveEquipment(const int clientId,
                     eEquipment& data) = 0;
    virtual bool
    unblockEquipment(const int clientId) = 0;

    virtual bool
    changeState(const int clientId,
                const eUnitData& u) = 0;
    virtual bool
    attack(const int clientId,
           const eAttackData& target) = 0;
    virtual bool
    stopAttack(const int clientId) = 0;

    virtual bool
    respawn(const int clientId) = 0;

    virtual bool
    setSkillId(const int clientId,
               const eSkillChoice schoice,
               const int skillId) = 0;

    virtual std::shared_ptr<eObject>
    triggerObject(const int clientId,
                  const int objectId,
                  const int tx, const int ty) = 0;

    virtual bool
    pickupItem(const int clientId,
               const int itemId,
               const bool drag) = 0;
    virtual bool
    dropItem(const int clientId) = 0;
    virtual bool
    rearrangeItems(const int clientId,
                   const eEquipment& eq) = 0;
    virtual bool
    changeAttributes(const int clientId,
                     const eAttributes& attrs) = 0;
    virtual bool
    changeSkillLevels(
        const int clientId,
        const eSkillLevels& skillLevels) = 0;
    virtual bool
    sendMessage(const int clientId,
                const std::string& text) = 0;

    virtual bool
    consumePotion(const int clientId,
                  const uint32_t itemId) = 0;
    virtual bool
    pickupBody(const int clientId,
               const int32_t bodyId) = 0;

    std::vector<eOtherUsers> receiveNewUsers();
    std::vector<int> receiveLeftUsers();
    std::vector<eMessage> receiveMessages();
    std::vector<eObject> receiveObjectStateChanges();
protected:
    void failed(const std::string& msg,
                const std::string& subMsg);

    std::vector<eOtherUsers> mNewUsers;
    std::vector<int> mLeftUsers;
    std::vector<eMessage> mMessages;
    std::vector<eObject> mObjectStateChanges;
private:
    eServerFailureHandler mFailure;
};

namespace eSlayerServer {
    ESLAYERSERVER_API std::shared_ptr<eServer>
    generate(const eServerData& data);
}

#endif // ESERVER_H

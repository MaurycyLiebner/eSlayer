#ifndef ESERVER_H
#define ESERVER_H

#include "eslayerserverexport.h"

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerHelpers/eskillchoice.h>
#include <eSlayerHelpers/emapportion.h>
#include <eSlayerHelpers/eslayers.h>

#include <memory>

class eRequestData;
class eCharacter;
struct eAttackData;
struct eWeaponData;
struct eBodyEquipment;
struct eEquipment;
struct eAttributes;
struct eSkillLevels;
struct eScreenDimensions;
struct eServerDoors;

struct eServerData {
    std::string fName;
    std::string fIp;
    std::string fPassword;
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

using eMapReadyAction = std::function<void(const eMapData& data)>;

class ESLAYERSERVER_API eServer {
public:
    eServer(const eServerData& data);

    void setFailureHandler(const eServerFailureHandler& h);

    virtual bool initialize() = 0;

    virtual int connect() = 0;
    virtual bool disconnect(const int clientId) = 0;

    virtual void increment(const float by) = 0;
    virtual void checkMapsReady() {}

    bool requestMapCall(const int clientId,
                        const uint8_t id,
                        const eMapReadyAction& func);
protected:
    virtual bool
    requestMap(const int clientId,
               const uint8_t id,
               const eMapReadyAction& func) = 0;
public:
    virtual bool
    spawn(const int clientId,
          eCharacter& c,
          eTeamId& teamId,
          ePointF& spawnPos,
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
    respawn(const int clientId,
            eBodyEquipment& beq,
            int& bodyId) = 0;

    virtual bool
    setSkillId(const int clientId,
               const eSkillChoice schoice,
               const int skillId) = 0;

    virtual bool
    triggerObject(const int clientId,
                  eServerObject& obj) = 0;

    virtual bool
    triggerDoors(const int clientId,
                 const eServerDoors& d) = 0;

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
               const uint32_t bodyId) = 0;

    virtual bool
    teamAction(const int clientId,
               const eTeamAction& action) = 0;

    uint8_t mapId() const { return mMapId; }

    const std::string& ip() const { return mData.fIp; }
    const std::string& name() const { return mData.fName; }
    const std::string& password() const { return mData.fPassword; }

    std::vector<eSlayer> receiveNewUsers();
    std::vector<int> receiveLeftUsers();
    std::vector<eMessage> receiveMessages();
    std::vector<eServerObject> receiveObjectStateChanges();
    std::vector<eServerDoors> receiveDoorsStateChanges();
    std::vector<uint32_t> receiveBodiesPickedUp();
protected:
    void failed(const std::string& msg,
                const std::string& subMsg);

    std::vector<eSlayer> mNewUsers;
    std::vector<int> mLeftUsers;
    std::vector<eMessage> mMessages;
    std::vector<eServerObject> mObjectStateChanges;
    std::vector<eServerDoors> mDoorsStateChanged;
    std::vector<uint32_t> mBodiesPickedUp;
private:
    const eServerData mData;
    uint8_t mMapId;

    eServerFailureHandler mFailure;
};

namespace eSlayerServer {
    ESLAYERSERVER_API std::shared_ptr<eServer>
    generate(const eServerData& data);
}

#endif // ESERVER_H

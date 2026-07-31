#ifndef ESERVER_H
#define ESERVER_H

#include "eslayerserverexport.h"

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerHelpers/eskillchoice.h>
#include <eSlayerHelpers/emapportion.h>
#include <eSlayerHelpers/eslayers.h>
#include <eSlayerHelpers/ebody.h>
#include <eSlayerHelpers/eequipmentaction.h>
#include <eSlayerHelpers/esellers.h>
#include <eSlayerHelpers/eslayerquests.h>
#include <eSlayerHelpers/etalk.h>
#include <eSlayerHelpers/emercenary.h>
#include <eSlayerHelpers/efollowers.h>

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
struct eMoveToMapData;

struct eServerData {
    std::string fName;
    std::string fIp;
    std::string fPassword;
    bool fChooseDifficulty = false;
    int fDifficulty = -1;
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

    virtual uint32_t connect() = 0;
    virtual bool disconnect(const uint32_t clientId) = 0;

    virtual bool setupGame(const eSlayerQuests& quests) = 0;

    virtual void increment(const float by) = 0;
    virtual void checkMapsReady() {}

    bool requestMapCall(const uint32_t clientId,
                        const eMoveToMapData& moveData,
                        const eMapReadyAction& func);
protected:
    virtual bool
    requestMap(const uint32_t clientId,
               const eMoveToMapData& moveData,
               const eMapReadyAction& func) = 0;
public:
    virtual bool
    spawn(const uint32_t clientId,
          eCharacter& c,
          eTeamId& teamId,
          ePointF& spawnPos,
          const eScreenDimensions& screenDims) = 0;

    virtual bool
    requestData(const uint32_t clientId,
                eRequestData& data,
                float& resultTime) = 0;
    virtual bool
    requestEquipment(const uint32_t clientId) = 0;
    virtual bool
    receiveEquipment(const uint32_t clientId,
                     eEquipment& data) = 0;
    virtual bool
    unblockEquipment(const uint32_t clientId) = 0;

    virtual std::optional<eSlayerQuests>
    receiveQuests(const uint32_t clientId) = 0;

    virtual bool
    heardTalk(const uint32_t clientId,
              const eConvoId& talk) = 0;

    virtual bool
    addedSocket(const uint32_t clientId,
                const uint8_t questId) = 0;

    virtual bool
    changeState(const uint32_t clientId,
                const eUnitData& u) = 0;
    virtual bool
    attack(const uint32_t clientId,
           const eAttackData& target) = 0;
    virtual bool
    stopAttack(const uint32_t clientId) = 0;

    virtual bool
    createBody(const uint32_t clientId) = 0;

    virtual bool
    setSkillId(const uint32_t clientId,
               const eSkillChoice schoice,
               const int skillId) = 0;

    virtual bool
    triggerObject(const uint32_t clientId,
                  const eServerObject& obj) = 0;

    virtual bool
    triggerNPC(const uint32_t clientId,
               const uint32_t npcId) = 0;

    virtual bool
    triggerDoors(const uint32_t clientId,
                 const eServerDoors& d) = 0;

    virtual bool
    pickupItem(const uint32_t clientId,
               const uint32_t itemId,
               const bool drag) = 0;
    virtual bool
    dropItem(const uint32_t clientId) = 0;
    virtual bool
    dropGold(const uint32_t clientId,
             const uint32_t count) = 0;
    virtual bool
    rearrangeItems(const uint32_t clientId,
                   const eEquipment& eq) = 0;
    virtual bool
    changeAttributes(const uint32_t clientId,
                     const eAttributes& attrs) = 0;
    virtual bool
    changeSkillLevels(
        const uint32_t clientId,
        const eSkillLevels& skillLevels) = 0;
    virtual bool
    sendMessage(const uint32_t clientId,
                const std::string& text) = 0;

    virtual bool
    consumePotion(const uint32_t clientId,
                  const uint32_t itemId,
                  const uint32_t unitId) = 0;
    virtual bool
    pickupBody(const uint32_t clientId,
               const uint32_t bodyId) = 0;

    virtual bool
    teamAction(const uint32_t clientId,
               const eTeamAction& action) = 0;

    virtual bool
    spawnPortal(const uint32_t clientId) = 0;

    virtual bool
    equipmentAction(const uint32_t clientId,
                    const eEquipmentAction& a) = 0;

    virtual bool
    buyAction(const uint32_t clientId,
              const eBuyAction& a) = 0;

    virtual bool
    sellAction(const uint32_t clientId,
               const eSellAction& a) = 0;

    virtual bool
    requestSeller(const uint32_t clientId,
                  const uint32_t sellerId) = 0;

    virtual bool
    summonMerc(const uint32_t clientId,
               const eMercenary& merc) = 0;

    virtual std::optional<eFollowersBase>
    followersUpdate(const uint32_t clientId) = 0;

    static uint32_t serverState() { return sServerState; }
    static void incServerState() { sServerState++; }

    const std::string& ip() const { return mData.fIp; }
    const std::string& name() const { return mData.fName; }
    const std::string& password() const { return mData.fPassword; }

    virtual std::vector<eSlayer> receiveNewUsers();
    virtual std::vector<eSlayer> receiveLeftUsers();
    virtual std::vector<eSlayer> receiveSlainUsers();
    virtual std::vector<eMessage> receiveMessages();
    virtual std::vector<eServerObject> receiveObjectStateChanges();
    virtual std::vector<eServerDoors> receiveDoorsStateChanges();
    virtual std::vector<uint32_t> receiveBodiesPickedUp();
    virtual std::vector<eBody> receiveBodiesCreated();
    virtual std::vector<eBodyItemsTaken> receiveBodiesChanged();
    virtual std::vector<eEquipmentAction> receiveEqActions();
    virtual std::optional<eSeller> receiveSeller();
    virtual std::optional<eReplaceItemId> receiveReplaceItemId();
protected:
    void failed(const std::string& msg,
                const std::string& subMsg);

    std::vector<eSlayer> mNewUsers;
    std::vector<eSlayer> mLeftUsers;
    std::vector<eSlayer> mSlainUsers;
    std::vector<eMessage> mMessages;
    std::vector<eServerObject> mObjectStateChanges;
    std::vector<eServerDoors> mDoorsStateChanged;
    std::vector<uint32_t> mBodiesPickedUp;
    std::vector<eBody> mBodiesCreated;
    std::vector<eBodyItemsTaken> mBodyItemsTaken;
    std::vector<eEquipmentAction> mEqActions;
    std::optional<eSeller> mSeller;
    std::optional<eReplaceItemId> mReplaceItemId;

    uint32_t mClientId = 0;
private:
    const eServerData mData;
    static uint32_t sServerState;

    eServerFailureHandler mFailure;
};

namespace eSlayerServer {
    ESLAYERSERVER_API std::shared_ptr<eServer>
    generate(const eServerData& data);
}

#endif // ESERVER_H

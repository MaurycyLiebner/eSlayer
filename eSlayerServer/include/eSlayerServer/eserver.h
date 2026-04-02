#ifndef ESERVER_H
#define ESERVER_H

#include "eslayerserverexport.h"

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerHelpers/eskillchoice.h>

#include <memory>

class eRequestData;
class eCharacter;
struct eAttackData;
struct eWeaponData;
struct eEquipment;
struct eScreenDimensions;

struct eServerData {
    std::string fName;
    std::string fIp;
};

using eServerFailureHandler = std::function<void(const std::string& msg,
                                                 const std::string& subMsg)>;

class ESLAYERSERVER_API eServer {
public:
    void setFailureHandler(const eServerFailureHandler& h);

    virtual bool initialize() = 0;

    virtual int connect() = 0;
    virtual bool disconnect(const int clientId) = 0;

    virtual void increment(const float by) = 0;

    virtual std::shared_ptr<eMap>
    requestMap(const int clientId,
               const std::string& name) = 0;
    virtual bool
    spawn(const int clientId,
          eCharacter& c,
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

    virtual bool
    pickupItem(const int clientId,
               const int itemId,
               const bool drag) = 0;
    virtual bool
    dropItem(const int clientId,
             const int itemId) = 0;
    virtual bool
    rearrangeItems(const int clientId,
                   const eEquipment& eq) = 0;
protected:
    void failed(const std::string& msg,
                const std::string& subMsg);

private:
    eServerFailureHandler mFailure;
};

namespace eSlayerServer {
    ESLAYERSERVER_API std::shared_ptr<eServer>
    generate(const eServerData& data);
}

#endif // ESERVER_H

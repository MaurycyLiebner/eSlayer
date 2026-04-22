#include "elocalserver.h"

#include <eSlayerHelpers/echaracter.h>

bool eLocalServer::initialize() {
    return true;
}

int eLocalServer::connect() {
    const int clientId = eServerUnit::sNextCharId++;
    mClientHandlers[clientId] = std::make_shared<eServerClientHandler>(clientId);
    return clientId;
}

bool eLocalServer::disconnect(const int clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    h->disconnect();
    mClientHandlers.erase(clientId);
    return true;
}

void eLocalServer::increment(const float by) {
    mTime += by;
    for(const auto& a : mAreas) {
        a.second->increment(by);
    }
}

bool eLocalServer::requestMap(
    const int clientId,
    const std::string& name,
    eMapData& data) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    const auto mapIt = mMaps.find(name);
    std::shared_ptr<eMap> map;
    if(mapIt == mMaps.end()) {
        map = eSlayerMapGenerator::generate(name);
        mMaps[name] = map;
    } else {
        map = mapIt->second;
    }
    const auto areaIt = mAreas.find(name);
    std::shared_ptr<eServerArea> area;
    if(areaIt == mAreas.end()) {
        area = std::make_shared<eServerArea>();
        area->initialize(map);
        mAreas[name] = area;
    } else {
        area = areaIt->second;
    }
    h->setArea(area);
    map->mapData(data);
    return true;
}

bool eLocalServer::spawn(const int clientId,
                         eCharacter& c,
                         const eScreenDimensions& screenDims) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    h->setName(c.name());
    return h->spawn(c, screenDims);
}

bool eLocalServer::requestData(const int clientId,
                               eRequestData& data,
                               float& resultTime) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->receiveData(data, resultTime);
}

bool eLocalServer::requestEquipment(const int clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->requestEquipment();
}

bool eLocalServer::receiveEquipment(const int clientId,
                                    eEquipment& data) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->receiveEquipment(data);
}

bool eLocalServer::unblockEquipment(const int clientId) {
    return true;
}

bool eLocalServer::changeState(
    const int clientId, const eUnitData& u) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->changeState(u);
}

bool eLocalServer::attack(const int clientId,
                          const eAttackData& target) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->attack(target);
}

bool eLocalServer::stopAttack(const int clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->stopAttack();
}

bool eLocalServer::respawn(const int clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->respawn();
}

bool eLocalServer::setSkillId(const int clientId,
                              const eSkillChoice schoice,
                              const int skillId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->setSkillId(schoice, skillId);
}

bool eLocalServer::pickupItem(
    const int clientId, const int itemId,
    const bool drag) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->pickupItem(itemId, drag);
}

bool eLocalServer::dropItem(
    const int clientId, const int itemId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->dropItem(itemId);
}

bool eLocalServer::rearrangeItems(
    const int clientId, const eEquipment& eq) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->rearrangeItems(eq);
}

bool eLocalServer::changeAttributes(
    const int clientId, const eAttributes& attrs) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->changeAttributes(attrs);
}

bool eLocalServer::changeSkillLevels(
    const int clientId, const eSkillLevels& skillLevels) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->changeSkillLevels(skillLevels);
}

bool eLocalServer::consumePotion(
    const int clientId, const uint32_t itemId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->consumePotion(itemId);
}

bool eLocalServer::pickupBody(
    const int clientId, const int32_t bodyId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->pickupBody(bodyId);
}

eServerClientHandler*
eLocalServer::clientHandler(const int clientId) {
    const auto it = mClientHandlers.find(clientId);
    if(it == mClientHandlers.end()) return nullptr;
    return it->second.get();
}


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

std::shared_ptr<eMap> eLocalServer::requestMap(
    const int clientId, const std::string& name) {
    const auto h = clientHandler(clientId);
    if(!h) return nullptr;
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
    return map;
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

eServerClientHandler*
eLocalServer::clientHandler(const int clientId) {
    const auto it = mClientHandlers.find(clientId);
    if(it == mClientHandlers.end()) return nullptr;
    return it->second.get();
}


#include "elocalserver.h"

#include "eserverchardata.h"

void eLocalServer::initialize() {
    eServerCharData::load();
}

int eLocalServer::connect() {
    const int clientId = eServerUnit::sNextCharId++;
    mClientHandlers[clientId] = std::make_shared<eServerClientHandler>(clientId);
    return clientId;
}

bool eLocalServer::disconnect(const int clientId) {
    mClientHandlers.erase(clientId);
    return true;
}

void eLocalServer::increment(const double by) {
    mTime += by;
    for(const auto& a : mAreas) {
        a->increment(by);
    }
}

std::shared_ptr<eMap> eLocalServer::requestMap(const int clientId, const std::string& name) {
    const auto h = clientHandler(clientId);
    if(!h) return nullptr;
    const auto it = mMaps.find(name);
    if(it != mMaps.end()) return it->second;
    const auto map = eSlayerMapGenerator::generate(name);
    mMaps[name] = map;
    const auto area = std::make_shared<eServerArea>();
    area->initialize(map);
    mAreas.push_back(area);
    h->setArea(area);
    return map;
}

bool eLocalServer::requestUnits(const int clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->requestUnits();
}

bool eLocalServer::receiveUnits(const int clientId,
                                std::vector<eUnitData>& units,
                                double& resultTime) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->receiveUnits(units, resultTime, mTime);
}

bool eLocalServer::moveTo(
    const int clientId, const ePointF& pos) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->moveTo(pos);
}

bool eLocalServer::attack(const int clientId,
                                 const int targetId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->attack(targetId);
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

eServerClientHandler*
eLocalServer::clientHandler(const int clientId) {
    const auto it = mClientHandlers.find(clientId);
    if(it == mClientHandlers.end()) return nullptr;
    return it->second.get();
}


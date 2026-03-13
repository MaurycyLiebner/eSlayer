#include "esingleplayerserver.h"

#include "eserverchardata.h"

void eSinglePlayerServer::initialize() {
    eServerCharData::load();
}

int eSinglePlayerServer::connect() {
    const int clientId = eServerUnit::sNextCharId++;
    mClientHandlers[clientId] = std::make_shared<eServerClientHandler>();
    return clientId;
}

bool eSinglePlayerServer::disconnect(const int clientId) {
    mClientHandlers.erase(clientId);
    return true;
}

void eSinglePlayerServer::increment(const double by) {
    mTime += by;
    for(const auto& a : mAreas) {
        a->increment(by);
    }
}

std::shared_ptr<eMap> eSinglePlayerServer::requestMap(const int clientId, const std::string& name) {
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

bool eSinglePlayerServer::requestUnits(const int clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return -1;
    return h->requestUnits();
}

int eSinglePlayerServer::receiveUnits(const int clientId,
                                      std::vector<eUnitData>& units,
                                      double& resultTime) {
    const auto h = clientHandler(clientId);
    if(!h) return -1;
    return h->receiveUnits(units, resultTime, mTime);
}

bool eSinglePlayerServer::moveTo(
    const int clientId, const ePointF& pos) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->moveTo(clientId, pos);
}

bool eSinglePlayerServer::attack(const int clientId,
                                 const int targetId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->attack(clientId, targetId);
}

bool eSinglePlayerServer::stopAttack(const int clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->stopAttack(clientId);
}

eServerClientHandler*
eSinglePlayerServer::clientHandler(const int clientId) {
    const auto it = mClientHandlers.find(clientId);
    if(it == mClientHandlers.end()) return nullptr;
    return it->second.get();
}

#include "esingleplayerserver.h"

int eSinglePlayerServer::connect() {
    const int clientId = mNextClientId++;
    mClientHandlers[clientId] = std::make_shared<eServerClientHandler>();
    return clientId;
}

bool eSinglePlayerServer::disconnect(const int clientId) {
    mClientHandlers.erase(clientId);
    return true;
}

void eSinglePlayerServer::increment() {
    for(const auto& a : mAreas) {
        a->increment();
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
                                      std::vector<std::shared_ptr<eServerUnit>>& units) {
    const auto h = clientHandler(clientId);
    if(!h) return -1;
    return h->receiveUnits(units);
}

eServerClientHandler*
eSinglePlayerServer::clientHandler(const int clientId) {
    const auto it = mClientHandlers.find(clientId);
    if(it == mClientHandlers.end()) return nullptr;
    return it->second.get();
}

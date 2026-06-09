#include "elocalserver.h"

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/echaracter.h>

#include <thread>

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
    for(const auto& it : mMaps) {
        const auto& ma = it.second;
        const auto& a = ma.fArea;
        a->removeClient(clientId);
    }
    h->disconnect();
    mClientHandlers.erase(clientId);
    return true;
}

void eLocalServer::increment(const float by) {
    for(const auto& it : mMaps) {
        const auto& ma = it.second;
        const auto& area = ma.fArea;
        area->increment(by);
    }
}

bool eLocalServer::requestMap(
    const int clientId,
    const uint8_t mapId,
    const eMapReadyAction& func) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    const auto mapIt = mMaps.find(mapId);
    std::shared_ptr<eMap> map;
    std::shared_ptr<eServerArea> area;
    const auto ofunc = [this, func, clientId](const eMapAndArea& ma) {
        const auto h = clientHandler(clientId);
        if(!h) return;
        const auto& area = ma.fArea;
        const auto& carea = h->area();
        eMapData data;
        const auto& map = ma.fMap;
        map->mapData(data);
        if(carea) {
            eServerArea::moveClient(clientId, *carea, *area, data.fSpawnPos);
        }
        h->setArea(area);
        func(data);
        return;
    };
    if(mapIt == mMaps.end()) {
        const auto it = mMapReadyActions.find(mapId);
        if(it == mMapReadyActions.end()) {
            std::thread t([this, mapId]() {
                const auto map = eSlayerMapGenerator::generate(mapId);
                const auto area = std::make_shared<eServerArea>();
                area->initialize(map);
                const eMapAndArea result{mapId, map, area};
                mReady.with_lock([&](std::vector<eMapAndArea>& v) {
                    v.emplace_back(result);
                });
            });
            t.detach();
        }
        mMapReadyActions[mapId].emplace_back(ofunc);
    } else {
        const auto& ma = mapIt->second;
        ofunc(ma);
    }
    return true;
}

bool eLocalServer::spawn(const int clientId,
                         eCharacter& c,
                         eTeamId& teamId,
                         const eScreenDimensions& screenDims) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    h->setName(c.name());
    return h->spawn(c, teamId, screenDims);
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

bool eLocalServer::respawn(const int clientId,
                           eBodyEquipment& beq,
                           int& bodyId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->respawn(beq, bodyId);
}

bool eLocalServer::setSkillId(const int clientId,
                              const eSkillChoice schoice,
                              const int skillId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->setSkillId(schoice, skillId);
}

std::shared_ptr<eObject> eLocalServer::triggerObject(
    const int clientId, const int objectId,
    const int tx, const int ty) {
    const auto h = clientHandler(clientId);
    if(!h) return nullptr;
    const auto obj = h->triggerObject(objectId, tx, ty);
    if(obj) mObjectStateChanges.emplace_back(*obj);
    return obj;
}

bool eLocalServer::triggerDoors(
    const int clientId, const eDoors& doors) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    const bool r = h->triggerDoors(doors);
    if(r) mDoorsStateChanged.emplace_back(doors);
    return r;
}

bool eLocalServer::pickupItem(
    const int clientId, const int itemId,
    const bool drag) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->pickupItem(itemId, drag);
}

bool eLocalServer::dropItem(
    const int clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->dropItem();
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
    const int clientId, const uint32_t bodyId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    const bool r = h->pickupBody(bodyId);
    if(r) mBodiesPickedUp.emplace_back(bodyId);
    return r;
}

void eLocalServer::mapReady(const eMapAndArea& ma) {
    const auto id = ma.fId;
    mMaps[id] = ma;
    const auto& as = mMapReadyActions[id];
    for(const auto& a : as) {
        a(ma);
    }
    mMapReadyActions.erase(id);
}

void eLocalServer::checkMapsReady() {
    mReady.with_lock([this](std::vector<eMapAndArea>& v) {
        for(const auto& ma : v) {
            mapReady(ma);
        }
        v.clear();
    });
}

int eLocalServer::clientMapId(const int clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return -1;
    const auto& a = h->area();
    if(!a) return -1;
    const auto& map = a->map();
    return map->id();
}

eServerClientHandler*
eLocalServer::clientHandler(const int clientId) {
    const auto it = mClientHandlers.find(clientId);
    if(it == mClientHandlers.end()) return nullptr;
    return it->second.get();
}


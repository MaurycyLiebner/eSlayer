#include "elocalserver.h"

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/eportals.h>

#include <thread>

bool eLocalServer::initialize() {
    return true;
}

uint32_t eLocalServer::connect() {
    const uint32_t clientId = eServerUnit::sNextCharId++;
    mClientHandlers[clientId] = std::make_shared<eServerClientHandler>(clientId);
    return clientId;
}

bool eLocalServer::disconnect(const uint32_t clientId) {
    eTeams::disconnect(clientId);
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
    const uint32_t clientId,
    const eMoveToMapData& moveData,
    const eMapReadyAction& func) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    const auto mapId = moveData.fMapId;
    std::shared_ptr<eMap> map;
    std::shared_ptr<eServerArea> area;
    const auto ofunc = [this, func, clientId, moveData](const eMapAndArea& ma) {
        const auto h = clientHandler(clientId);
        if(!h) return;
        const auto& area = ma.fArea;
        const auto& carea = h->area();
        eMapData data;
        const auto& map = ma.fMap;
        map->mapData(data);
        if(carea) {
            eServerArea::moveClient(clientId, *carea, *area, moveData, data.fSpawnPos);
            const auto mapId = map->id();
            eSlayers::setLocation(clientId, mapId, data.fSpawnPos);
        }
        h->setArea(area);
        func(data);
        return;
    };
    requestMap(mapId, ofunc);
    return true;
}

bool eLocalServer::spawn(const uint32_t clientId,
                         eCharacter& c,
                         eTeamId& teamId,
                         ePointF& spawnPos,
                         std::vector<eBody>& bodies,
                         const eScreenDimensions& screenDims) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    const bool r = h->spawn(c, teamId, spawnPos, bodies, screenDims);
    if(!r) return false;
    const auto& name = c.name();
    eSlayer slayer;
    slayer.fClientId = clientId;
    const auto map = h->map();
    slayer.fMapId = map->id();
    slayer.fPos = spawnPos;
    slayer.fName = name;
    eSlayers::sSlayers[clientId] = slayer;
    return true;
}

bool eLocalServer::requestData(const uint32_t clientId,
                               eRequestData& data,
                               float& resultTime) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->receiveData(data, resultTime);
}

bool eLocalServer::requestEquipment(const uint32_t clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->requestEquipment();
}

bool eLocalServer::receiveEquipment(const uint32_t clientId,
                                    eEquipment& data) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->receiveEquipment(data);
}

bool eLocalServer::unblockEquipment(const uint32_t clientId) {
    return true;
}

bool eLocalServer::changeState(
    const uint32_t clientId, const eUnitData& u) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    if(u.getUpdate(eUnitData::eShift::position)) {
        eSlayers::setPoisition(clientId, u.fPos);
    }
    return h->changeState(u);
}

bool eLocalServer::attack(const uint32_t clientId,
                          const eAttackData& target) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->attack(target);
}

bool eLocalServer::stopAttack(const uint32_t clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->stopAttack();
}

bool eLocalServer::respawn(const uint32_t clientId,
                           uint32_t& bodyId,
                           ePointF& bodyPos) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->respawn(bodyId, bodyPos);
}

bool eLocalServer::setSkillId(const uint32_t clientId,
                              const eSkillChoice schoice,
                              const int skillId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->setSkillId(schoice, skillId);
}

bool eLocalServer::triggerObject(
    const uint32_t clientId, const eServerObject& obj) {
    eServerObject changed = obj;
    return triggerObjectImpl(clientId, changed);
}

bool eLocalServer::triggerObjectImpl(
    const uint32_t clientId, eServerObject& obj) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    const bool r = h->triggerObject(obj);
    if(r) mObjectStateChanges.emplace_back(obj);
    return true;
}

bool eLocalServer::triggerDoors(
    const uint32_t clientId, const eServerDoors& doors) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    const bool r = h->triggerDoors(doors);
    if(r) mDoorsStateChanged.emplace_back(doors);
    return r;
}

bool eLocalServer::pickupItem(
    const uint32_t clientId,
    const uint32_t itemId,
    const bool drag) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->pickupItem(itemId, drag);
}

bool eLocalServer::dropItem(
    const uint32_t clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->dropItem();
}

bool eLocalServer::rearrangeItems(
    const uint32_t clientId, const eEquipment& eq) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->rearrangeItems(eq);
}

bool eLocalServer::changeAttributes(
    const uint32_t clientId, const eAttributes& attrs) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->changeAttributes(attrs);
}

bool eLocalServer::changeSkillLevels(
    const uint32_t clientId, const eSkillLevels& skillLevels) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->changeSkillLevels(skillLevels);
}

bool eLocalServer::consumePotion(
    const uint32_t clientId, const uint32_t itemId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->consumePotion(itemId);
}

bool eLocalServer::pickupBody(
    const uint32_t clientId,
    const uint32_t bodyId) {
    bool bodyRemoved;
    eBody body;
    const bool r = pickupBody(
        clientId, bodyId, bodyRemoved, body);
    if(r) {
        if(bodyRemoved) {
            mBodiesPickedUp.emplace_back(bodyId);
        } else {
            mBodiesChanged.emplace_back(body);
        }
    }
    return r;
}

bool eLocalServer::pickupBody(
    const uint32_t clientId,
    const uint32_t bodyId,
    bool& bodyRemoved,
    eBody& body) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->pickupBody(bodyId, bodyRemoved, body);
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

bool eLocalServer::requestMap(
    const uint8_t mapId,
    const eMapReadyBaseAction& func) {
    const auto mapIt = mMaps.find(mapId);
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
        mMapReadyActions[mapId].emplace_back(func);
    } else {
        const auto& ma = mapIt->second;
        func(ma);
    }
    return true;
}

void eLocalServer::checkMapsReady() {
    mReady.with_lock([this](std::vector<eMapAndArea>& v) {
        for(const auto& ma : v) {
            mapReady(ma);
        }
        v.clear();
    });
}

bool eLocalServer::teamAction(
    const uint32_t clientId, const eTeamAction& action) {
    switch(action.fType) {
    case eTeamActionType::makeEnemies:
        return eTeams::makeEnemies(action.fTeamId, clientId);
    case eTeamActionType::makeFriends:
        return eTeams::makeFriends(action.fTeamId, clientId);
    case eTeamActionType::invite:
        return eTeams::invite(action.fInvitedId, clientId);
    case eTeamActionType::cancelInvite:
        return eTeams::cancelInvite(action.fInvitedId, clientId);
    case eTeamActionType::acceptInvitation: {
        const bool r = eTeams::acceptInvitation(action.fTeamId, clientId);
        if(!r) return false;
        return eLocalServer::changeTeam(clientId, action.fTeamId);
    } break;
    case eTeamActionType::leaveTeam:
        const auto newTeamId = eTeams::leaveTeam(clientId);
        return eLocalServer::changeTeam(clientId, newTeamId);
    }
    return false;
}

bool eLocalServer::spawnPortal(const uint32_t clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    ePortal p;
    p.fCreator = clientId;
    const bool r = h->spawnPortal(
        p.fOutdoorPortalId,
        p.fOutdoorMapId,
        p.fOutdoorAreaId,
        p.fOutdoorPos);
    if(!r) return false;
    const auto map = h->map();
    if(!map) return false;
    const auto mapId = map->id();
    const auto& info = eMapsSettings::sMaps.get(mapId);
    const auto rmap = info.fRespawnMap;
    return requestMap(rmap, [clientId, p](const eMapAndArea& ma) {
        ePortal p2(p);
        const auto& a = ma.fArea;
        const bool r = a->spawnCampPortal(
            clientId,
            p2.fCampPortalId,
            p2.fCampMapId,
            p2.fCampAreaId,
            p2.fCampPos);
        if(r) ePortal::addPortal(p2);
    });
}

bool eLocalServer::changeTeam(
    const uint32_t clientId, const eTeamId newTeam) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->changeTeam(newTeam);
}

int eLocalServer::clientMapId(const uint32_t clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return -1;
    const auto& a = h->area();
    if(!a) return -1;
    const auto& map = a->map();
    return map->id();
}

eServerClientHandler*
eLocalServer::clientHandler(const uint32_t clientId) {
    const auto it = mClientHandlers.find(clientId);
    if(it == mClientHandlers.end()) return nullptr;
    return it->second.get();
}


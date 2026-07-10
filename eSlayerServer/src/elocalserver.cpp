#include "elocalserver.h"

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/eportals.h>
#include <eSlayerHelpers/esellers.h>

#include <thread>

bool eLocalServer::initialize() {
    return true;
}

uint32_t eLocalServer::connect() {
    mClientId = eServerUnit::sNextCharId++;
    mClientHandlers[mClientId] = std::make_shared<eServerClientHandler>(mClientId);
    return mClientId;
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
    uint8_t mapId;
    if(moveData.fType == eMoveToMapType::respawn) {
        const auto& carea = h->area();
        if(!carea) return false;
        const auto& cmap = carea->map();
        const auto cmapId = cmap->id();
        const auto& info = eMapsSettings::sMaps.get(cmapId);
        mapId = info.fRespawnMap;
    } else {
        const auto& to = moveData.fTo;
        mapId = to.fMapId;
    }
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
            const bool r = eServerArea::moveClient(
                clientId, *carea, *area,
                moveData, data.fSpawnPos);
            if(!r) {
                disconnect(clientId);
                return;
            }
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
                         const eScreenDimensions& screenDims) {
    std::vector<eBody> bodies;
    const bool r = spawnImpl(
        clientId, c, teamId, spawnPos,
        bodies, screenDims);
    if(!r) return false;
    mBodiesCreated = bodies;
    return true;
}

bool eLocalServer::spawnImpl(
    const uint32_t clientId,
    eCharacter& c,
    eTeamId& teamId,
    ePointF& spawnPos,
    std::vector<eBody>& bodies,
    const eScreenDimensions& screenDims) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    const bool r = h->spawn(c, teamId, spawnPos,
                            bodies, screenDims);
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

std::optional<eSlayerQuests>
eLocalServer::receiveQuests(const uint32_t clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return std::nullopt;
    return h->receiveQuests();
}

bool eLocalServer::heardTalk(
    const uint32_t clientId,
    const eConvoId& talk) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->heardTalk(talk);
}

bool eLocalServer::addedSocket(
    const uint32_t clientId,
    const uint8_t questId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->addedSocket(questId);
}

bool eLocalServer::receiveEquipment(
    const uint32_t clientId,
    eEquipment& data) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->receiveEquipment(data);
}

bool eLocalServer::unblockEquipment(
    const uint32_t clientId) {
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

bool eLocalServer::createBody(
    const uint32_t clientId) {
    eBody body;
    const bool r = createBodyImpl(clientId, body);
    if(!r) return false;
    mBodiesCreated.emplace_back(body);
    return true;
}

bool eLocalServer::createBodyImpl(
    const uint32_t clientId, eBody& body) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->createBody(body);
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
    eEquipmentAction action;
    const bool r = pickupItemImpl(
        clientId, itemId, drag, action);
    if(!r) return false;
    mEqActions.emplace_back(action);
    return true;
}

bool eLocalServer::pickupItemImpl(
    const uint32_t clientId,
    const uint32_t itemId,
    const bool drag,
    eEquipmentAction& action) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    const bool r = h->pickupItem(
        itemId, drag, action);
    return r;
}

bool eLocalServer::dropItem(
    const uint32_t clientId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->dropItem();
}

bool eLocalServer::dropGold(
    const uint32_t clientId,
    const uint32_t count) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->dropGold(count);
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
    const uint32_t clientId,
    const uint32_t itemId,
    const uint32_t unitId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->consumePotion(itemId, unitId);
}

bool eLocalServer::pickupBody(
    const uint32_t clientId,
    const uint32_t bodyId) {
    bool bodyRemoved;
    eBodyItemsTaken taken;
    const bool r = pickupBodyImpl(
        clientId, bodyId, bodyRemoved, taken);
    if(r) {
        if(bodyRemoved) {
            mBodiesPickedUp.emplace_back(bodyId);
        }
        mBodyItemsTaken.emplace_back(taken);
    }
    return r;
}

bool eLocalServer::pickupBodyImpl(
    const uint32_t clientId,
    const uint32_t bodyId,
    bool& bodyRemoved,
    eBodyItemsTaken& taken) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->pickupBody(bodyId, bodyRemoved, taken);
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
        p.fOutdoorArea,
        p.fOutdoorPos);
    if(!r) return false;
    const auto map = h->map();
    if(!map) return false;
    const auto mapId = map->id();
    const auto& info = eMapsSettings::sMaps.get(mapId);
    const auto actId = info.fActId;
    ePortal::removeCreatorActPortal(clientId, actId);
    const auto rmap = info.fRespawnMap;
    return requestMap(rmap, [clientId, p](const eMapAndArea& ma) {
        ePortal p2(p);
        const auto& a = ma.fArea;
        const bool r = a->spawnCampPortal(
            clientId,
            p2.fCampPortalId,
            p2.fCampArea,
            p2.fCampPos);
        if(r) ePortal::addPortal(p2);
    });
}

bool eLocalServer::equipmentAction(
    const uint32_t clientId,
    const eEquipmentAction& a) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->equipmentAction(a);
}

bool eLocalServer::buyAction(
    const uint32_t clientId,
    const eBuyAction& a) {
    uint32_t newItemId;
    const bool r = buyActionImpl(clientId, a, newItemId);
    if(!r) return false;
    eReplaceItemId i;
    i.fSellerId = a.fSellerId;
    i.fOldItemId = a.fItemId;
    i.fNewItemId = newItemId;
    mReplaceItemId = i;
    return true;
}

bool eLocalServer::buyActionImpl(
    const uint32_t clientId,
    const eBuyAction& a,
    uint32_t& newItemId) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->buyAction(a, newItemId);
}

bool eLocalServer::sellAction(
    const uint32_t clientId,
    const eSellAction& a) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->sellAction(a);
}

bool eLocalServer::requestSeller(
    const uint32_t clientId,
    const uint32_t sellerId) {
    eSeller s;
    const bool r = requestSellerImpl(clientId, sellerId, s);
    if(!r) return false;
    mSeller = s;
    return true;
}

bool eLocalServer::requestSellerImpl(
    const uint32_t clientId,
    const uint32_t sellerId,
    eSeller& seller) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->requestSeller(sellerId, seller);
}

bool eLocalServer::summonMerc(
    const uint32_t clientId,
    const eMercenary& merc) {
    eMercenary result = merc;
    const bool r = summonMercImpl(clientId, result);
    if(!r) return false;
    mMerc = result;
    return true;
}

bool eLocalServer::summonMercImpl(
    const uint32_t clientId,
    eMercenary& merc) {
    const auto h = clientHandler(clientId);
    if(!h) return false;
    return h->summonMerc(merc);
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


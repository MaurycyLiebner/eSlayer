#include "eserverarea.h"

#include <eSlayerHelpers/eunitsinfo.h>

#include <eSlayerMapGenerator/emap.h>

bool eServerArea::spawnBody(const uint32_t clientId,
                            const eBodyEquipment& beq,
                            uint32_t& bodyId,
                            ePointF& spawnPos) {
    const auto client = unit(clientId);
    if(!client) return false;
    const auto& data = client->data();
    const int typeId = 0;
    const auto u = std::make_shared<eServerUnit>(
        eUnitType::slayerBody, data, typeId, *this);
    eEquipment bodyEq;
    static_cast<eBodyEquipment&>(bodyEq) = beq;
    u->setEquipment(bodyEq, false);
    const auto& udata = eUnitsInfo::sUnits.get(typeId);
    const uint32_t charId = eServerUnit::sNextCharId++;
    const auto& modelParts = client->fModelParts;
    const auto teamId = client->fTeamId;
    spawnPos = client->fPos;
    iniSetupUnit(u, charId, teamId, spawnPos,
                 typeId, udata, data, modelParts);
    u->fHealth = 0;
    u->fAnim = data.animId("body");
    bodyId = charId;
    mBodies[clientId].emplace_back(charId);
    return true;
}

bool eServerArea::createBody(
    const uint32_t clientId,
    eBody& body) {
    const auto client = unit(clientId);
    if(!client) return false;
    const bool createBody = true;
    if(!createBody) return false;
    auto& eq = client->equipment();
    const auto beq = eq.takeBody();
    body.fMapId = mMap->id();
    body.fEq = beq;
    const bool r = spawnBody(
        clientId, beq,
        body.fBodyId, body.fPos);

    dropGold(clientId, eq.fInventoryGold);

    return r;
}

bool eServerArea::pickupBody(
    const uint32_t clientId,
    const uint32_t bodyId,
    bool& bodyRemoved,
    eBodyItemsTaken& taken) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& client = it->second;
    auto& bodies = mBodies[clientId];
    const auto bit = std::find(bodies.begin(), bodies.end(), bodyId);
    if(bit == bodies.end()) return false;
    const auto ubody = unit(bodyId);
    if(!ubody) return false;
    const auto u = unit(clientId);
    if(!u || u->fHealth <= 0) return false;
    const float dist = ePointF::distance(ubody->fPos, u->fPos);
    if(dist > 1.f) return false;
    auto& dst = u->equipment();
    auto& src = ubody->equipment();
    dst.moveFromBody(src, &taken.fItems);
    taken.fBodyId = bodyId;
    bodyRemoved = src.bodyEmpty();
    if(bodyRemoved) {
        bodies.erase(bit);
        planRemoveUnit(bodyId);
    }
    u->recalculateStats();
    u->recalculateAuras();
    u->refreshModelParts();
    return true;
}
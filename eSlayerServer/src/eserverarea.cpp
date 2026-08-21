#include "eserverarea.h"

#include "actions/eunitbaseaction.h"
#include "actions/efolloweraction.h"
#include "actions/enpcaction.h"
#include "actions/ewaitaction.h"

#include "eelitemodifiers.h"

#include "eitemgenerator.h"

#include <eSlayerMissiles/emissileincrementer.h>
#include <eSlayerMissiles/emissilecollision.h>
#include <eSlayerMissiles/emissileincrement.h>

#include <eSlayerMapGenerator/emap.h>

#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/evectorhelpers.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/eobjectsinfo.h>
#include <eSlayerHelpers/eitemsdata.h>
#include <eSlayerHelpers/edoors.h>
#include <eSlayerHelpers/eportals.h>
#include <eSlayerHelpers/esellers.h>
#include <eSlayerHelpers/equests.h>
#include <eSlayerHelpers/emercenaries.h>
#include <eSlayerHelpers/edifficulties.h>
#include <eSlayerHelpers/eclasses.h>

std::vector<uint32_t> eServerArea::sSlain;
std::map<uint32_t, std::shared_ptr<eServerUnit>>
eServerArea::sSlayers;
eSlayerQuests eServerArea::sGameQuests;

eServerArea::eServerArea() :
    mMIncrementer(mUnitAreas),
    mNIncrementer(mUnitAreas) {
    iniMissileInc();
    iniNovaInc();
}

void eServerArea::iniSetupFollowerAction(
    const std::shared_ptr<eServerUnit>& u,
    const std::shared_ptr<eServerUnit>& follow) {
    const auto a = std::make_shared<eFollowerAction>(*u, *this, follow);
    u->setAction(a);
}

void eServerArea::setQuestStageCount(const uint32_t clientId, eClientData& data,
                                     const uint8_t questId, const uint8_t stageId,
                                     const int count) {
    auto& qs = data.fQuests;
    qs.setCount(questId, stageId, count);

    const auto u = unit(clientId);
    if(u) {
        const auto skillPoints = qs.receiveSkillPoints();
        if(skillPoints > 0) {
            u->addSkillPoints(skillPoints);
        }

        const auto statPoints = qs.receiveStatPoints();
        if(statPoints > 0) {
            u->addStatPoints(statPoints);
            u->setAttributesChanged(true);
        }
    }

    data.fSendQuests = true;
    checkQuestItems(clientId);
}

void eServerArea::checkEnterArea(const uint32_t clientId) {
    const auto cit = mClientData.find(clientId);
    if(cit == mClientData.end()) return;
    auto& clientData = cit->second;
    const auto u = unit(clientId);
    if(!u) return;
    const auto& upos = u->fPos;
    const auto areaId = mMap->areaAt(upos);
    const auto& qs = eQuests::sEnterAreaQuests;
    const auto it = qs.find(areaId);
    if(it == qs.end()) return;
    for(const auto& q : it->second) {
        auto& qs = clientData.fQuests;
        const auto stage = qs.stage(q.fQuestId);
        if(stage > q.fStageId) continue;
        setQuestStageCount(clientId, clientData, q.fQuestId, q.fStageId, 1);
    }
}

void eServerArea::updateGlobalQuestCount(
    const eQuestStepId step, const uint8_t count) {
    const auto questId = step.fQuestId;
    const auto stage = step.fStageId;
    const auto current = sGameQuests.count(
        questId, stage);
    if(current >= count) return;
    sGameQuests.setCount(questId, stage, count);

    {
        std::set<uint32_t> show;
        for(const auto uid : mHiddenUnits) {
            const auto u = unit(uid);
            if(!u) continue;
            const auto infoId = u->fUnitInfoId;
            const bool v = sGameQuests.npcVisible(infoId);
            if(v) show.emplace(uid);
        }

        for(const auto uid : show) {
            mHiddenUnits.erase(uid);
            const auto u = unit(uid);
            if(!u) continue;
            showUnit(*u);
        }
    }

    {
        std::set<uint32_t> hide;
        for(const auto uid : mFutureHideUnits) {
            const auto u = unit(uid);
            if(!u) continue;
            const auto infoId = u->fUnitInfoId;
            const bool v = sGameQuests.npcVisible(infoId);
            if(!v) hide.emplace(uid);
        }

        for(const auto uid : hide) {
            mFutureHideUnits.erase(uid);
            const auto u = unit(uid);
            if(!u) continue;
            const auto a = u->action();
            if(const auto npcA = dynamic_cast<eNPCAction*>(a.get())) {
                npcA->cure();
            } else {
                hideUnit(*u);
            }
        }
    }
}

void eServerArea::hideUnit(eServerUnit& u) {
    u.setVisible(false);
}

void eServerArea::showUnit(eServerUnit& u) {
    findPlaceForUnit(u.fPos, u.fPos);
    u.setVisible(true);
}

void eServerArea::usedSkills(const uint32_t clientId,
                             std::set<int>& usedSkills) {
    const auto client = unit(clientId);
    if(!client) return;
    usedSkills = client->takeUsedSkills();
}

eArea eServerArea::unitArea(const uint32_t charId) const {
    const auto u = unit(charId);
    if(!u) return {0, 0};
    return unitArea(*u);
}

eArea eServerArea::unitArea(const eServerUnit& u) const {
    const auto& pos = u.fPos;
    return mUnitAreas.posArea(pos);
}

eArea eServerArea::itemArea(const uint32_t itemId) const {
    const auto i = groundItem(itemId);
    if(!i) return {0, 0};
    return itemArea(*i);
}

eArea eServerArea::itemArea(const eGroundItem& i) const {
    const auto& pos = i.fPos;
    return mItemAreas.posArea(pos);
}

eArea eServerArea::itemTile(const uint32_t itemId) const {
    const auto i = groundItem(itemId);
    if(!i) return {0, 0};
    return itemTile(*i);
}

eArea eServerArea::itemTile(const eGroundItem& i) const {
    const auto& pos = i.fPos;
    return mItemTiles.posArea(pos);
}

bool eServerArea::walkable(
    const ePointF& pos) const {
    return mMap->walkable(pos);
}

bool eServerArea::walkable(
    const ePointF& from, const ePointF& to) const {
    return mMap->walkable(from, to);
}

bool eServerArea::obstacle(
    const ePointF& pos) const {
    return mMap->obstacle(pos);
}

bool eServerArea::obstacle(
    const ePointF& from, const ePointF& to) const {
    return mMap->obstacle(from, to);
}

bool eServerArea::findPlaceForPortal(
    const ePointF& pos, ePointF& result) const {
    const float x = pos.fX;
    const float y = pos.fY;

    const auto portalType = eObjectsInfo::sObjects.id("portal");

    const auto valid = [&](const ePointF& pos) {
        for(int dx = -1; dx <= 1; dx++) {
            for(int dy = -1; dy <= 1; dy++) {
                const int x = pos.fX + dx;
                const int y = pos.fY + dy;
                const bool r = mMap->inside(x, y);
                if(!r) continue;
                const auto& oIds = mMap->objects(x, y);
                for(const auto oId : oIds) {
                    const auto& o = mMap->object(oId);
                    if(portalType != o->fObjectType) continue;
                    if(o->inside(pos)) return false;
                }
            }
        }
        return true;
    };

    for(int dist = 0; dist < 5; dist++) {
        const int maxTries = dist == 0 ? 1 : 10;
        for(int i = 0; i <= maxTries; i++) {
            const float dx = eRand::randF(-dist, dist);
            const float dy = eRand::randF(-dist, dist);
            const ePointF tryPos{x + dx, y + dy};
            const bool w = walkable(tryPos);
            if(!w) continue;
            const bool r = valid(tryPos);
            if(!r) continue;
            result = tryPos;
            return true;
        }
    }
    return false;
}

bool eServerArea::requestSeller(
    const uint32_t clientId,
    const uint32_t sellerId,
    eSeller& seller) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& ss = eSellers::sSellers;
    auto it = ss.find(sellerId);
    if(it == ss.end()) return false;
    auto& s = it->second;
    {
        auto& cp = s.fClientPage;
        const auto it = cp.find(clientId);
        if(it == cp.end()) {
            auto& p = s.addClientPage(clientId);
            const auto& potionTypes = s.fSellPotionTypes;
            for(const auto ptype : potionTypes) {
                const auto& itemData = eItemsData::get(ptype);
                if(itemData.fType != eItemType::potion) continue;
                eItem item;
                eItemGenerator::applyItemId(item);
                const auto type = itemData.fType;
                item.fDataId = ptype;
                item.fType = type;
                item.fSubType = itemData.fSubtype;
                p.tryAdd(item);
            }
        }
    }
    seller = s;
    return true;
}

uint32_t eServerArea::nearestCorpse(
    const ePointF& pos,
    const eValidator& v) const {
    uint32_t result = 0;
    const float maxDist = 3.f;
    float minDist = maxDist;
    const auto iter = [&](
        const std::shared_ptr<eServerUnit>& u) {
        const bool c = u->isCorpse();
        if(!c) return false;
        if(v && !v(*u)) return false;
        const auto& upos = u->fPos;
        const float dist = ePointF::distance(upos, pos);
        if(dist > minDist) return false;
        minDist = dist;
        result = u->fCharId;
        return false;
    };
    iterateOverUnits(pos, maxDist, iter);
    return result;
}

std::optional<eFollowersBase>
eServerArea::followersUpdate(const uint32_t clientId) {
    const auto u = unit(clientId);
    if(!u) return std::nullopt;
    const auto& eq = u->equipment();
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return std::nullopt;
    auto& c = it->second;
    const auto& src = u->followers();
    if(src.fState <= c.fFollowersState) return std::nullopt;
    c.fFollowersState = src.fState;
    return src;
}

void eServerArea::clear() {
    sSlayers.clear();
    sSlain.clear();
    eServerUnit::sNextCharId = 1;
    eItemGenerator::clear();
}

bool eServerArea::iterateOverClients(
    const eIter& iter) {
    for(auto& it : mClientData) {
        const auto clientId = it.first;
        auto& data = it.second;
        const bool r = iter(clientId, data);
        if(r) return true;
    }
    return false;
}

bool eServerArea::iterateOverClients(
    const ePointF& pos,
    const float maxDist,
    const eIter& iter) {
    return iterateOverClients([&](
            const uint32_t clientId,
            eClientData& data) {
        const auto u = unit(clientId);
        if(!u) return false;
        if(u->fHealth <= 0) return false;
        const auto upos = u->fPos;
        const float dist = ePointF::distance(upos, pos);
        if(dist > maxDist) return false;
        return iter(clientId, data);
    });
}

bool eServerArea::checkQuestItems(
    const uint32_t clientId) {
    const auto u = unit(clientId);
    if(!u) return false;
    const auto& eq = u->equipment();
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& c = it->second;
    const auto& iqs = eQuests::sFindItemQuests;
    for(const auto& it : iqs) {
        const auto itemId = it.first;

        uint8_t count = 0;
        eq.iterateOverAll([&](const eItem& item) {
            if(item.fDataId == itemId) count++;
        });

        const auto& qids = it.second;
        auto& qs = c.fQuests;
        for(const auto& qid : qids) {
            const auto questId = qid.fQuestId;
            const auto stageId = qid.fStageId;
            const bool r = qs.setCount(
                questId, stageId, count);
            updateGlobalQuestCount(qid, count);
            if(r) c.fSendQuests = true;
        }
    }
    return true;
}

bool goThroughPortal(
    const uint32_t clientId,
    const uint32_t portalId) {
    const auto p = ePortal::portal(portalId);
    if(!p) return false;
    if(p->fCreator != clientId) return false;
    if(p->fCampPortalId != portalId) return false;
    ePortal::removePortal(portalId);
    return true;
}

bool eServerArea::moveClient(
    const uint32_t clientId,
    eServerArea& from,
    eServerArea& to,
    const eMoveToMapData& moveData,
    ePointF& spawnPos) {
    const auto u = from.unit(clientId);
    if(!u) return false;
    const auto clientData = from.mClientData[clientId];
    std::vector<std::shared_ptr<eServerUnit>> followers;
    for(const auto f : u->followers()) {
        const auto u = from.unit(f);
        if(!u) continue;
        followers.emplace_back(u);
    }
    if(&from != &to) from.clientMoved(clientId);
    const bool r = to.addClient(
        clientId, u, followers,
        clientData, moveData, spawnPos);
    if(!r) return false;
    if(moveData.fType == eMoveToMapType::portal) {
        goThroughPortal(clientId, moveData.fPortalId);
    }
    return true;
}

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

bool eServerArea::removeClient(const uint32_t clientId) {
    sSlayers.erase(clientId);
    planRemoveUnit(clientId);
    const auto& bodies = mBodies[clientId];
    for(const auto bodyId : bodies) {
        planRemoveUnit(bodyId);
    }
    mBodies.erase(clientId);
    const int r = mClientData.erase(clientId);
    return r > 0;
}

bool eServerArea::clientMoved(const uint32_t clientId) {
    removeUnit(clientId);
    const int r = mClientData.erase(clientId);
    return r > 0;
}

bool eServerArea::planRemoveUnit(const uint32_t charId) {
    mUnitsToRemove.emplace_back(charId);
    return true;
}

bool eServerArea::removeUnit(const uint32_t charId) {
    const auto area = unitArea(charId);
    if(!mUnitAreas.hasArea(area)) return false;
    mUnitAreas.erase(area, charId);
    mUnits.remove(charId);
    return true;
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

bool eServerArea::changeTeam(
    const uint32_t clientId, const eTeamId newTeam) {
    const auto u = unit(clientId);
    if(!u) return false;
    u->setTeamId(newTeam);
    const auto& followers = u->followers();
    for(const auto f : followers) {
        const auto u = unit(f);
        if(!u) continue;
        u->setTeamId(newTeam);
    }
    return true;
}

bool eServerArea::spawnPortal(const uint32_t clientId,
                              uint32_t& portalId,
                              eAreaIds& area,
                              ePointF& pos) {
    const auto u = unit(clientId);
    if(!u) return false;
    pos = u->fPos;
    return spawnPortal(pos, portalId, area);
}

bool eServerArea::spawnCampPortal(
    const uint32_t clientId,
    uint32_t& portalId,
    eAreaIds& area,
    ePointF& pos) {
    pos = mMap->portalSpawnPos();
    return spawnPortal(pos, portalId, area);
}

bool eServerArea::triggerNPC(
    const uint32_t clientId,
    const uint32_t npcId) {
    const auto client = unit(clientId);
    if(!client) return false;
    const auto npc = unit(npcId);
    if(!npc) return false;
    const auto infoId = npc->fUnitInfoId;
    const auto& info = eUnitsInfo::sUnits.get(infoId);
    switch(info.fNPCType) {
    case eNPCType::healer: {
        client->healAll();
    } break;
    default:
        break;
    }
    return true;
}

bool eServerArea::spawnPortal(
    ePointF& pos,
    uint32_t& portalId,
    eAreaIds& area) {
    const bool r = findPlaceForPortal(pos, pos);
    if(!r) return false;
    const auto typeId = eObjectsInfo::sObjects.id("portal");
    const auto& info = eObjectsInfo::sObjects.get(typeId);
    const auto o = mMap->addObject(pos, info.fWidth, info.fHeight);
    o->fObjectType = typeId;
    o->fSubtype = 0;
    portalId = o->fObjectId;
    area.fMapId = mMap->id();
    area.fAreaId = mMap->areaAt(pos);
    return true;
}

bool eServerArea::triggerObject(
    const uint32_t clientId, eServerObject& obj) {
    const auto u = unit(clientId);
    if(!u) return false;
    if(obj.fMapId != mMap->id()) return false;
    const auto& pos = obj.fPos;
    const int tx = pos.fX;
    const int ty = pos.fY;
    if(!mMap->inside(tx, ty)) return false;
    const int areaId = mMap->areaAt(pos);
    if(areaId < 0) return false;
    const auto& areaSett = eMapsSettings::sAreas.get(areaId);
    const int diff = eDifficulties::sDifficulty;
    const auto level = areaSett.template_(diff).fLevel;
    const auto& objIds = mMap->objects(tx, ty);
    for(const auto id : objIds) {
        const auto& sobj = mMap->object(id);
        const auto objId = sobj->fObjectId;
        if(objId != sobj->fObjectId) continue;
        const auto type = sobj->fObjectType;
        const auto& info = eObjectsInfo::sObjects.get(type);
        if(info.fKey >= 0) {
            const auto& eq = u->equipment();
            bool found = false;
            eq.iterateOverAll([&](const eItem& item) {
                if(item.fDataId == info.fKey) {
                    found = true;
                }
            });
            if(!found) return false;
        }
        switch(info.fType) {
        case eObjectType::treasure: {
            auto& state = sobj->fState;
            if(state != 0) return false;
            const float fx = tx + sobj->fWidth + 0.5f;
            const ePointF pos{fx, float(ty)};
            generateItems(pos, level, 7.5f);
            generateItems(pos, info.fItemDrops);

            state = 1;
        } break;
        case eObjectType::trapDoor: {
            auto& state = sobj->fState;
            if(state != 0) return false;
            state = 1;
        } break;
        default:
            break;
        }
        static_cast<eObject&>(obj) = *sobj;
        return true;
    }
    return false;
}

bool eServerArea::triggerDoors(
    const uint32_t clientId, const eServerDoors& doors) {
    if(doors.fMapId != mMap->id()) return false;
    mMap->triggerDoors(doors);
    return true;
}

bool eServerArea::pickupItem(
    const uint32_t clientId,
    const uint32_t itemId,
    const bool drag,
    eEquipmentAction& action) {
    const auto u = unit(clientId);
    if(!u) return false;
    const auto itemPtr = mItemsOnGround.get(itemId);
    if(!itemPtr) return false;
    auto& item = action.fAddItem;
    action.fUnitId = clientId;
    item = *itemPtr;
    const auto gitem = mGroundItems.get(itemId);
    const auto area = itemArea(itemId);
    const auto tile = itemTile(itemId);
    auto& eq = u->equipment();
    if(gitem->fType == eItemType::gold) {
        eq.fInventoryGold += item.fCount;

        action.fType = eEquipmentActionType::gold;
        action.fInvGold = eq.fInventoryGold;
        action.fStashGold = eq.fStashGold;
    } else {
        action.fType = eEquipmentActionType::add;
        auto& aplace = action.fPlace;
        if(drag) {
            if(eq.fDragged.fType != eItemType::none) return false;
            aplace.fType = ePlaceType::dragged;
            eq.fDragged = item;
        } else {
            const auto& stats = u->stats();
            const bool met = stats.itemReqsMet(item);
            const bool r = eq.add(item, met, &aplace);
            if(!r) return false;
            u->recalculateStats();
            u->recalculateAuras();
            u->refreshModelParts();
        }

        checkQuestItems(clientId);
    }
    mGroundItems.remove(itemId);
    mItemsOnGround.remove(itemId);
    mItemAreas.erase(area, itemId);
    mItemTiles.erase(tile, itemId);
    return true;
}

bool eServerArea::dropItem(const uint32_t clientId) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    const auto pos = u->fPos;
    const auto tryDropItem = [&](eItem& item) {
        if(item.fType != eItemType::none) {
            addGroundItem(pos, item);
            item = eItem();
        }
    };
    tryDropItem(eq.fDragged);
    tryDropItem(eq.fTemporary);
    checkQuestItems(clientId);
    return true;
}

bool eServerArea::dropGold(const uint32_t clientId,
                           uint32_t count) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    const auto pos = u->fPos;
    count = std::min(eq.fInventoryGold, count);
    if(count <= 0) return false;
    eq.fInventoryGold -= count;
    const auto item = eItemGenerator::generateGold(count);
    addGroundItem(pos, item);
    return true;
}

void eServerArea::rearrangeItems(
    const uint32_t clientId, const eEquipment& eq) {
    const auto u = unit(clientId);
    if(!u) return;
    u->setEquipment(eq);
}

bool eServerArea::equipmentAction(
    const uint32_t clientId,
    const eEquipmentAction& a) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    if(a.fUnitId == clientId) {
        const bool r = a.apply(eq, eq.fDragged);
        u->recalculateStats();
        u->refreshModelParts();
        return r;
    } else {
        const auto u = unit(a.fUnitId);
        if(!u) return false;
        auto& ueq = u->equipment();
        const bool r = a.apply(ueq, eq.fDragged);
        u->recalculateStats();
        return r;
    }
}

bool eServerArea::buyAction(
    const uint32_t clientId,
    const eBuyAction& a,
    uint32_t& newItemId) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    auto item = eSellers::item(
        clientId, a.fSellerId, a.fItemId);
    const uint32_t gold = item.calculateCost();
    const uint32_t hgold = eq.totalGold();
    if(gold > hgold) return false;
    if(item.fType == eItemType::none) return false;
    const bool r = eEquipmentAction::add(
        eq, item, a.fPlace);
    if(!r) return false;
    u->recalculateStats();
    u->refreshModelParts();
    eq.takeGold(gold);
    if(item.fType == eItemType::potion) {
        eReplaceItemId r;
        r.fSellerId = a.fSellerId;
        r.fOldItemId = item.fItemId;
        eItemGenerator::applyItemId(item);
        newItemId = item.fItemId;
        r.fNewItemId = newItemId;
        eSellers::replaceItemId(
            clientId, r);
    } else {
        newItemId = 0;
        eSellers::takeItem(
            clientId, a.fSellerId, a.fItemId);
    }
    return true;
}

bool eServerArea::sellAction(
    const uint32_t clientId,
    const eSellAction& a) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& ss = eSellers::sSellers;
    const auto it = ss.find(a.fSellerId);
    if(it == ss.end()) return false;
    auto& s = it->second;
    auto& eq = u->equipment();
    const auto item = eq.take(a.fItemId);
    if(item.fType == eItemType::none) return false;
    const uint32_t gold = item.calculateSellCost();
    eq.fInventoryGold += gold;
    auto& p = s.fClientPage[clientId];
    p.tryAdd(item);
    return true;
}

void eServerArea::changeAttributes(
    const uint32_t clientId, const eAttributes& attrs) {
    const auto u = unit(clientId);
    if(!u) return;
    const auto& dst = u->attributes();
    const bool r = eAttributes::samePoints(attrs, dst);
    if(r) u->setAttributes(attrs);
}

void eServerArea::changeSkillLevels(
    const uint32_t clientId, const eSkillLevels& skillLevels) {
    const auto u = unit(clientId);
    if(!u) return;
    const auto& stats = u->stats();
    const bool r = stats.validLevelsChange(skillLevels);
    if(r) u->setSkillLevels(skillLevels);
}

void eServerArea::consumePotion(
    const uint32_t clientId,
    const uint32_t itemId,
    const uint32_t unitId) {
    const auto u = unit(clientId);
    if(!u) return;
    const auto p = u->takePotion(itemId);
    const auto tu = unit(unitId);
    if(!tu) return;
    tu->consumePotion(p);
}

bool eServerArea::boostsAurasChanged(const uint32_t clientId) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    clientData.fUpdateBoostsAuras = true;
    return true;
}

bool eServerArea::updateBoostsAuras(const uint32_t clientId) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    const bool result = clientData.fUpdateBoostsAuras;
    clientData.fUpdateBoostsAuras = false;
    return result;
}

bool eServerArea::heardTalk(
    const uint32_t clientId,
    const eConvoId& talk,
    std::vector<eEquipmentAction>& eqActions) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    const auto u = unit(clientId);
    if(!u) return false;
    auto& qs = clientData.fQuests;

    const auto& c = eTalks::get(talk);
    switch(c.fType) {
    case eConvoType::intro:
        return false;
    default:
        break;
    }

    const auto questId = c.fQuestId;
    const auto& qinfo = eQuests::sQuests.get(questId);
    const auto stage = c.fStageId;
    const bool intro = qinfo.introStage(stage);
    const bool outro = qinfo.outroStage(stage);
    uint8_t count;
    bool allClientsFulfill;
    if(intro || outro) {
        count = 1;
        allClientsFulfill = false;
    } else {
        const auto stepId = qinfo.stageToStep(stage);
        const auto& step = qinfo.fSteps[stepId];
        count = step.fCount;
        allClientsFulfill = step.fAllClientsFulfill;
        if(step.fType == eQuestType::bringItem) {
            auto& eq = u->equipment();
            std::vector<uint32_t> items;
            eq.iterateOverAll([&](const eItem& item) {
                if(items.size() >= step.fCount) return;
                if(item.fDataId == step.fTargetItem) {
                    items.emplace_back(item.fItemId);
                }
            });
            if(items.size() >= step.fCount) {
                for(const auto itemId : items) {
                    eq.take(itemId);
                }
            } else {
                return false;
            }
        } else if(step.fType == eQuestType::getItem) {
            auto& eq = u->equipment();

            const int typeId = step.fTargetItem;

            for(int i = 0; i < step.fCount; i++) {
                const auto item = eItemGenerator::generateItem(
                    typeId, 1, step.fItemWorth);
                if(item.fType == eItemType::none) {
                    return false;
                }
                eEquipmentPlace place;
                const bool r = eq.add(item, true, &place);
                if(r) {
                    auto& a = eqActions.emplace_back();
                    a.fType = eEquipmentActionType::add;
                    a.fPlace = place;
                    a.fAddItem = item;
                    a.fUnitId = u->fCharId;
                } else {
                    const auto& pos = u->fPos;
                    addGroundItem(pos, item);
                }
            }
        }
    }

    const auto iter = [&](const uint32_t clientId, eClientData& data) {
        setQuestStageCount(clientId, data, questId, stage, count);
        return false;
    };

    if(allClientsFulfill) {
        const auto& upos = u->fPos;
        iterateOverClients(upos, 15.f, iter);
    } else {
        iter(clientId, clientData);
    }

    const eQuestStepId qsId{questId, stage};
    updateGlobalQuestCount(qsId, count);

    return true;
}

bool eServerArea::addedSocket(
    const uint32_t clientId,
    const uint8_t questId) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    const auto u = unit(clientId);
    if(!u) return false;
    auto& clientData = it->second;
    auto& qs = clientData.fQuests;
    const bool r = qs.isAddSocketStage(questId);
    if(!r) return false;
    auto& eq = u->equipment();
    auto& tmp = eq.fTemporary;
    const bool rr = tmp.addSocket();
    if(!rr) return false;
    const auto stage = qs.stage(questId);
    uint8_t count = 0;
    qs.incCount(questId, stage, &count);
    checkQuestItems(clientId);
    clientData.fSendQuests = true;
    const eQuestStepId qsId{questId, stage};
    updateGlobalQuestCount(qsId, count);
    return true;
}

void eServerArea::addSkillArea(
    const std::shared_ptr<eServerSkillArea>& a) {
    mSkillAreas.add(a->fId, a);
}

void eServerArea::addMissile(const std::shared_ptr<eServerMissile>& m) {
    mMissiles.add(m->fId, m);
}

void eServerArea::addNova(const std::shared_ptr<eServerNova>& n) {
    mNovas.add(n->fId, n);
}

uint32_t eServerArea::findOtherTarget(
    const eServerUnit& u,
    const float range,
    const std::set<uint32_t>& skip) {
    uint32_t result = 0;
    const auto uid = u.fCharId;
    const auto& upos = u.fPos;
    iterateOverUnitsClamped(upos, range, [&](
        const std::shared_ptr<eServerUnit>& u) {
        const auto id = u->fCharId;
        if(id == uid) return false;
        if(skip.count(id) > 0) return false;
        result = id;
        return true;
    });
    return result;
}

uint32_t eServerArea::findMinOtherTarget(
    const eServerUnit& u,
    const float range,
    const std::map<uint32_t, uint8_t>& skip) {
    uint32_t result = 0;
    uint8_t min = 255;
    const auto uid = u.fCharId;
    const auto& upos = u.fPos;
    iterateOverUnitsClamped(upos, range, [&](
        const std::shared_ptr<eServerUnit>& u) {
        const auto id = u->fCharId;
        if(id == uid) return false;
        const auto it = skip.find(id);
        if(it == skip.end()) {
            min = 0;
            result = id;
            return true;
        }
        const auto imin = it->second;
        if(imin < min) {
            min = imin;
            result = id;
        }
        return false;
    });
    return result;
}

int piercedFromPierceChance(const float p) {
    if(p <= 0.f) return 0;
    if(p >= 1.f) return std::numeric_limits<int>::max();
    const float u = eRand::randF();
    return int(std::log(u) / std::log(p));
}

void eServerArea::spawnMissile(const ePointF& to,
                               const eSkill& skill,
                               const eHitData& data,
                               const int nMissiles,
                               const float pierceChance,
                               const int missileId,
                               const float range,
                               const float radius,
                               const float time,
                               const bool continuousDamage,
                               const int consecutive) {
    const auto skillType = skill.fType;
    const auto twinBehaviour = skill.fTwinBehaviour;
    const bool avoid = twinBehaviour != eTwinBehaviour::none;
    auto baseDir = ePointF::vector(to, data.fFrom);
    if(baseDir.length() < 0.001f) baseDir = eVec2f::random();
    struct eMissileData {
        ePointF fPos;
        ePointF fTo;
        int fToPierce;
        int fMissileId;
        float fRange;
        float fTime;
        eDamage fDamage;
    };
    std::vector<eMissileData> missiles;
    const auto spawnMissiles = [&](const int missileId,
                                   const float range) {
        float maxAngle = skill.fMaxAngle;
        if(nMissiles > 1 && maxAngle == 0.f) {
            maxAngle = 30.f*(nMissiles - 1);
        }
        if(skill.fAngleAdjust) {
            if(range > 0.f) {
                const float len = baseDir.length();
                const float multBase = 1.f - 3.f*len/range;
                const float angleMult = std::clamp(multBase, 0.1f, 1.f);
                maxAngle *= angleMult;
            }
        }
        float angle = nMissiles == 1 ? 0.f : -0.5f*maxAngle;
        for(int i = 0; i < nMissiles; i++) {
            auto dir = baseDir;
            if(angle != 0.f) dir.rotate(angle);
            auto& md = missiles.emplace_back();
            const int max = std::numeric_limits<uint8_t>::max();
            const int pierced = piercedFromPierceChance(pierceChance);
            md.fToPierce = std::min(max, 1 + pierced);
            md.fPos = data.fFrom;
            md.fTo = data.fFrom + dir;
            md.fMissileId = missileId;
            md.fRange = range;
            md.fDamage = data.fDamage;
            if(nMissiles > 1) {
                angle += maxAngle/(nMissiles - 1);
            }
        }
    };
    if(skillType == eSkillType::missile) {
        spawnMissiles(missileId, range);
    } else if(skillType == eSkillType::wall) {
        eVec2f perp(-baseDir.y, baseDir.x);
        perp.normalize(2*radius);
        ePointF pt = to - perp * (nMissiles/2);
        for(int i = 0; i < nMissiles; i++) {
            auto& md = missiles.emplace_back();
            md.fToPierce = 0;
            md.fPos = pt;
            md.fTo = pt;
            md.fMissileId = skill.fMissileId;
            md.fTime = skill.fTime;
            md.fRange = 0.f;
            md.fDamage = data.fDamage;
            pt = pt + perp;
        }
    } else {
        spawnMissiles(missileId, range);
    }
    std::vector<std::shared_ptr<eServerMissile>> twins;
    for(const auto& md : missiles) {
        const auto m = std::make_shared<eServerMissile>();
        if(avoid) twins.emplace_back(m);
        auto& mref = *m;
        mref.fType = md.fMissileId;
        mref.fTeamId = data.fAttackTeamId;
        mref.fToPierce = md.fToPierce;
        mref.fSpeed = skill.fSpeed;

        mref.fTotalDist = md.fRange;
        mref.fTotalTime = md.fTime;
        mref.fRemDist = mref.fTotalDist;
        mref.fRemTime = mref.fTotalTime;

        mref.fPathType = skill.fPathId;
        mref.fFrom = data.fFrom;
        mref.fRadius = radius;
        mref.fPos = md.fPos;
        mref.fTo = md.fTo;
        mref.fContinuousDamage = continuousDamage;
        mref.fConsecutive = consecutive;
        mref.fEnemyFindRange = skill.fMissileEnemyFindRange;
        mref.fTwinBehaviour = twinBehaviour;
        mref.fTime = 0.f;

        struct eCharSkipper {
            float fTimeSkip = 0.f;
            float fTime = 0.f;
            std::set<uint32_t> fChars;
        };

        std::shared_ptr<eCharSkipper> skip;
        if(continuousDamage) {
            skip = std::make_shared<eCharSkipper>();
        } else if(consecutive > 0) {
            skip = std::make_shared<eCharSkipper>();
            skip->fTimeSkip = 1000.f;
        }
        m->fHitAction = [data, skip](
            eServerUnit& u, const eServerMissile& m) {
            if(skip) {
                auto& c = skip->fChars;
                if(skip->fTime < m.fTime) {
                    c.clear();
                    skip->fTime = m.fTime + skip->fTimeSkip;
                } else {
                    if(c.find(u.fCharId) != c.end()) {
                        return;
                    }
                }
                c.emplace(u.fCharId);
            }
            u.getHit(data);
        };
        addMissile(m);
    }

    for(const auto& m : twins) {
        for(const auto& twin : twins) {
            if(m == twin) continue;
            const auto id = twin->fId;
            m->fTwinMissiles.emplace(id);
        }
    }
}

void eServerArea::spawnArea(const ePointF& to,
                            const eSkill& skill,
                            eHitData data,
                            const float radius,
                            const int missileId) {
    const int area = skill.fAreaMissileId;
    if(area <= 0) return;
    const auto a = std::make_shared<eServerSkillArea>();
    data.fDamage = data.fDamage/25.f;
    data.fHeal /= 25.f;
    a->fIncrement = [this, data, to, radius]() {
        const auto team = data.fAttackTeamId;
        iterateOverUnitsClamped(to, radius,
                [&](const std::shared_ptr<eServerUnit>& u) {
            if(u->fHealth <= 0) return false;
            const auto uteam = u->fTeamId;
            const auto& pos = u->fPos;
            if(team == uteam) {
                u->restoreHealth(data.fHeal);
            } else {
                if(!eTeams::areEnemies(team, uteam)) return false;
                u->getHit(data);
            }
            return false;
        });
    };
    a->fMissileId = area;
    a->fRemTime = skill.fTime;
    a->fRadius = radius;
    a->fPos = to;
    addSkillArea(a);
}

void eServerArea::spawnNova(const eSkill& skill,
                            const eHitData& data,
                            const float radius,
                            const bool continuousDamage) {
    const auto n = std::make_shared<eServerNova>();
    n->fTeamId = data.fAttackTeamId;
    n->fMissileType = skill.fMissileId;
    if(skill.fTargetCorpse) {
        const auto corpseId = nearestCorpse(data.fTo);
        if(!corpseId) return;
        const auto c = unit(corpseId);
        n->fCenter = c->fPos;
        c->explodeCorpse();
    } else {
        n->fCenter = data.fFrom;
    }
    n->fRadius = 0.f;
    n->fMaxRadius = radius;
    n->fSpeed = skill.fSpeed;
    n->fNMissiles = skill.fNovaMissiles;

    struct eCharSkipper {
        std::set<int> fChars;
    };

    const std::shared_ptr<eCharSkipper> skip =
        !continuousDamage ?
            std::make_shared<eCharSkipper>() :
            nullptr;
    n->fHitAction = [data, skip](eServerUnit& u) {
        if(skip) {
            auto& c = skip->fChars;
            if(c.find(u.fCharId) != c.end()) {
                return;
            }
            c.emplace(u.fCharId);
        }
        u.getHit(data);
    };
    addNova(n);
}

std::shared_ptr<eServerUnit>
eServerArea::summon(eServerUnit& by,
                    const uint32_t corpseId,
                    const int unitId,
                    const int maxCount,
                    const std::vector<eModifier>& mods) {
    const auto corpse = unit(corpseId);
    if(!corpse) return nullptr;
    const auto& to = corpse->fPos;

    const auto u = summon(by, to, unitId, maxCount, mods);
    if(u) {
        const auto raise = eWaitAction::sCreateRaise(
            *u, *this);
        if(raise) u->setChildAction(raise);
    }

    planRemoveUnit(corpseId);

    return u;
}

void eServerArea::raise(eServerUnit& by,
                        const uint32_t corpseId,
                        const int maxCount,
                        const std::vector<eModifier>& mods,
                        const bool follow) {
    const auto corpse = unit(corpseId);
    if(!corpse) return;
    const bool c = corpse->isCorpse();
    if(!c) return;
    auto& followers = by.followers();
    const auto raised = eServerArea::raised(by);
    if(maxCount > 0 && raised.size() >= maxCount) {
        const uint32_t removeCharId = raised[0];
        planRemoveUnit(removeCharId);
        followers.remove(removeCharId);
    }
    corpse->raise();
    corpse->setTeamId(by.fTeamId);
    corpse->addBoost(mods, eBoostCurseType::summon, true);
    if(follow) {
        const auto byPtr = unit(by.fCharId);
        iniSetupFollowerAction(corpse, byPtr);
    }
    const auto raise = eWaitAction::sCreateRaise(
        *corpse, *this);
    if(raise) corpse->setChildAction(raise);
}

std::shared_ptr<eServerUnit>
eServerArea::summon(eServerUnit& by,
                         ePointF to,
                         const int unitId,
                         const int maxCount,
                         const std::vector<eModifier>& mods) {
    const bool r = findPlaceForUnit(to, to);
    if(!r) return nullptr;
    auto& followers = by.followers();
    const auto summoned = eServerArea::summoned(by, unitId);
    if(maxCount > 0 && summoned.size() >= maxCount) {
        const uint32_t removeCharId = summoned[0];
        planRemoveUnit(removeCharId);
        followers.remove(removeCharId);
    }
    const auto& udata = eUnitsInfo::sUnits.get(unitId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    const auto modelParts = data.randomModelParts();

    const auto u = std::make_shared<eServerUnit>(
        eUnitType::summoned, data, unitId, *this);
    const uint32_t charId = eServerUnit::sNextCharId++;
    followers.add(charId);
    iniSetupUnit(u, charId, by.fTeamId, to,
                 unitId, udata, data, modelParts);
    u->addBoost(mods, eBoostCurseType::permanent, false);
    {
        const int schoice = u->addSkill();
        u->setSkillId(schoice, 0, false);
    }
    u->recalculateStats();
    u->recalculateAuras();

    const auto byPtr = unit(by.fCharId);
    iniSetupFollowerAction(u, byPtr);

    return u;
}

bool eServerArea::summonMerc(
    const uint32_t clientId,
    eMercenary merc) {
    const auto by = unit(clientId);
    if(!by) return false;
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    auto& cMerc = clientData.fMerc;
    if(cMerc) {
        planRemoveUnit(cMerc->fUnitId);
        cMerc = std::nullopt;
    }
    ePointF to = by->fPos;
    const bool r = findPlaceForUnit(to, to);
    if(!r) return false;
    auto& followers = by->followers();
    const auto& mdata = eMercenariesInfo::sMercs.get(merc.fMercType);
    const auto unitId = mdata.fUnitType;
    const auto& udata = eUnitsInfo::sUnits.get(unitId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    const auto modelParts = data.randomModelParts();

    const auto m = std::make_shared<eServerUnit>(
        eUnitType::mercenary, data, unitId, *this);
    m->setMercType(merc.fMercType);
    const auto mercId = eServerUnit::sNextCharId++;
    followers.add(mercId);
    iniSetupUnit(m, mercId, by->fTeamId, to,
                 unitId, udata, data, modelParts);
    {
        const int schoice = m->addSkill();
        m->setSkillId(schoice, 0, false);
    }

    merc.fUnitId = mercId;
    auto& eq = merc.fEq;
    eq.iterateOverAll([](eItem& item) {
        if(item.fType == eItemType::none) return;
        eItemGenerator::applyItemId(item);
    });
    m->setEquipment(eq, false);
    const auto attrs = merc.attributes();
    m->setAttributes(attrs, false);
    const auto mods = merc.mods();
    m->addBoost(mods, eBoostCurseType::merc, false);
    merc.fDead = false;
    cMerc = merc;

    m->recalculateStats();
    m->recalculateAuras();

    iniSetupFollowerAction(m, by);
    return true;
}

bool eServerArea::castChance(
    eServerUnit& by,
    const eSkillStats& o,
    const eWeaponChoice wchoice,
    const ePointF& to) {
    const bool r = eRand::randChance(o.fCastChance);
    if(!r) return false;
    cast(by, o, wchoice, to);
    return true;
}

void eServerArea::cast(eServerUnit& by,
                       const eSkillStats& o,
                       const eWeaponChoice wchoice,
                       const ePointF& to) {
    const auto& skill = eSkills::sSkills.get(o.fSkillId);
    eHitData data;
    by.hitData(o, wchoice, data);
    switch(skill.fType) {
    case eSkillType::missile:
    case eSkillType::wall: {
        const int nMissiles = by.skillCount(o, wchoice);
        const float pierceChance = by.pierceChance(o, wchoice);
        const int missileId = by.missileId(o, wchoice);
        const float missileRange = by.missileRange(o, wchoice);
        const float missileTime = by.missileTime(o, wchoice);
        const float radius = by.radius(o, wchoice);
        const bool continuousDamage = skill.fType == eSkillType::wall;
        const int consecutive = by.consecutive(o, wchoice);
        spawnMissile(to, skill, data,
                     nMissiles, pierceChance, missileId,
                     missileRange, radius, missileTime,
                     continuousDamage, consecutive);
    } break;
    case eSkillType::nova: {
        const float radius = by.radius(o, wchoice);
        const bool continuousDamage = false;
        spawnNova(skill, data, radius, continuousDamage);
    } break;
    case eSkillType::summon: {
        const int maxCount = by.skillCount(
            o, eWeaponChoice::left);
        const int unitId = skill.fUnitId;
        const auto summoned = eServerArea::summoned(by, unitId);
        const int currCount = summoned.size();
        if(maxCount > currCount) {
            summon(by, to, unitId, maxCount, {});
        }
    } break;
    case eSkillType::raise: {
        const int maxCount = by.skillCount(
            o, eWeaponChoice::left);
        uint32_t corpseId = 0;
        const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
            const bool c = u->isCorpse();
            if(!c) return false;
            corpseId = u->fCharId;
            return true;
        };
        iterateOverUnitsClamped(by.fPos, 5.f, iter);
        raise(by, corpseId, maxCount, {}, true);
    } break;
    case eSkillType::area:
    case eSkillType::boostCurse: {
        const float radius = by.radius(o, wchoice);
        spawnArea(to, skill, data, radius, skill.fMissileId);
    } break;
    case eSkillType::attack:
    case eSkillType::dualAttack:
    case eSkillType::aura:
    case eSkillType::shoot:
    case eSkillType::kick:
    case eSkillType::smite:
    case eSkillType::passive:
    case eSkillType::throw_:
        break;
    }

    switch(o.fExplode) {
    case eExplodeType::none:
        break;
    default:
        by.die(o.fExplode);
        break;
    }
}

std::vector<uint32_t> eServerArea::summoned(
    const eServerUnit& by, const int unitId) {
    std::vector<uint32_t> result;
    const auto& followers = by.followers();
    for(const auto charId : followers) {
        const auto u = unit(charId);
        const auto unitIdU = u->fUnitInfoId;
        if(unitIdU == unitId) {
            result.emplace_back(charId);
        }
    }
    return result;
}

std::vector<uint32_t> eServerArea::raised(
    const eServerUnit& by) {
    std::vector<uint32_t> result;
    const auto& followers = by.followers();
    for(const auto charId : followers) {
        const auto u = unit(charId);
        const bool r = u->isRaised();
        if(!r) continue;
        result.emplace_back(charId);
    }
    return result;
}

bool eServerArea::campAt(const ePointF& pos) const {
    return mMap->campAt(pos);
}

std::shared_ptr<eServerUnit>
eServerArea::unit(const uint32_t charId) const {
    return mUnits.get(charId);
}

std::shared_ptr<eServerUnit> eServerArea::unit(
    const ePointF& pos, const eValidator& validator) const {
    std::shared_ptr<eServerUnit> result;

    const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
        if(validator) {
            const bool r = validator(*u);
            if(!r) return false;
        }
        result = u;
        return true;
    };

    const float maxRadius = 1.f;
    iterateOverUnitsClamped(pos, maxRadius, iter);
    return result;
}

bool eServerArea::iterateOverUnits(const eArea& areaMin,
                                   const eArea& areaMax,
                                   const eUnitIter& iter) const {
    for(int ax = areaMin.fX; ax <= areaMax.fX; ax++) {
        for(int ay = areaMin.fY; ay <= areaMax.fY; ay++) {
            const eArea area{ax, ay};
            if(!mUnitAreas.hasArea(area)) continue;
            const auto& units = mUnitAreas.at(area);
            for(const uint32_t charId : units) {
                const auto u = unit(charId);
                if(!u) continue;
                const bool r = iter(u);
                if(r) return true;
            }
        }
    }
    return false;
}

bool eServerArea::iterateOverUnitsClamped(
    const ePointF& pos,
    const float maxRadius,
    const eUnitIter& iter) const {
    return iterateOverUnits(pos, maxRadius, [&](
        const std::shared_ptr<eServerUnit>& u) {
        const auto& upos = u->fPos;
        const double dist = ePointF::distance(pos, upos);
        if(dist > maxRadius) return false;
        return iter(u);
    });
}

bool eServerArea::iterateOverUnits(const ePointF& pos,
                                   const float maxRadius,
                                   const eUnitIter& iter) const {
    const float minX = pos.fX - maxRadius;
    const float maxX = pos.fX + maxRadius;
    const float minY = pos.fY - maxRadius;
    const float maxY = pos.fY + maxRadius;

    const auto areaMin = mUnitAreas.posArea(ePointF{minX, minY});
    const auto areaMax = mUnitAreas.posArea(ePointF{maxX, maxY});
    return iterateOverUnits(areaMin, areaMax, iter);
}

void eServerArea::unitKilled(const eServerUnit& killed) {
    {
        const bool merc = killed.isMercenary();
        if(merc) {
            iterateOverClients([&](const uint32_t clientId, eClientData& data) {
                if(data.fMerc) {
                    auto& merc = *data.fMerc;
                    if(merc.fUnitId == killed.fCharId) {
                        merc.setDead(true);
                        merc.setUnitId(0);
                        return true;
                    }
                }
                return false;
            });
        } else {
            const auto id = killed.fUnitInfoId;
            const auto& mqs = eQuests::sKillMonsterQuests;
            const auto it = mqs.find(id);
            iterateOverClients(killed.fPos, 15.f, [&](
                    const uint32_t clientId, eClientData& data) {
                if(it != mqs.end()) {
                    const auto& qs = it->second;
                    for(const auto& q : qs) {
                        const auto questId = q.fQuestId;
                        auto& qs = data.fQuests;
                        uint8_t count = 0;
                        const bool r = qs.incCount(
                            questId, q.fStageId, &count);
                        updateGlobalQuestCount(q, count);
                        if(r) {
                            checkQuestItems(clientId);
                            data.fSendQuests = true;
                        }
                    }
                }

                const auto u = unit(clientId);
                if(!u) return false;
                const eTeamId t1 = u->fTeamId;
                const eTeamId t2 = killed.fTeamId;
                if(!eTeams::areEnemies(t1, t2)) return false;
                u->killed(killed);

                if(data.fMerc) {
                    auto& merc = *data.fMerc;
                    const auto m = unit(merc.fUnitId);
                    if(m) {
                        const auto& attrs = m->attributes();
                        m->killed(killed);
                        merc.setExp(attrs.fExp);
                        merc.setLevel(attrs.fLevel);
                    }
                }

                return false;
            });
        }
    }

    const int level = killed.level();
    const auto type = killed.unitType();
    float worth = 0.f;
    switch(type) {
    case eUnitType::slayer: {
        sSlain.emplace_back(killed.fCharId);
        return;
    } break;
    case eUnitType::slayerBody:
        return;
    case eUnitType::mercenary:
    case eUnitType::summoned:
        break;
    case eUnitType::normal: {
        const bool gen = eRand::randChance(0.2f);
        if(gen) worth = eRand::biasedRandF(0.25f, 10.f, 8.f);
    } break;
    case eUnitType::minion: {
        const bool gen = eRand::randChance(0.2f);
        if(gen) worth = eRand::biasedRandF(0.25f, 10.f, 5.f);
    } break;
    case eUnitType::uniqueBoss: {
        worth = eRand::biasedRandF(2.f, 10.f, 3.f);
        for(int i = 0; i < 4; i++) {
            const float pworth = eRand::randF(0.25f, 1.f);
            generatePotion(killed.fPos, level, pworth);
        }
    } break;
    }

    const float fleeRange = 5.f;
    iterateOverUnitsClamped(killed.fPos, fleeRange,
        [&](const std::shared_ptr<eServerUnit>& u) {
        if(u->fHealth <= 0) return false;
        if(u->fTeamId != killed.fTeamId) return false;
        if(u->fTeamId != eTeamId::neutralFriendly &&
           u->fTeamId != eTeamId::neutralHostile &&
           u->fTeamId != eTeamId::neutral) return false;
        const auto& info = eUnitsInfo::sUnits.get(u->fUnitInfoId);
        if(eRand::randChance(info.fFleeChance)) {
            const auto& a = u->action();
            if(const auto ua = dynamic_cast<eUnitBaseAction*>(&*a)) {
                const eFlee flee{killed.fPos, info.fFleeDistance};
                ua->planFlee(flee);
            }
        }
        return false;
    });

    if(worth > 0.f) generateItem(killed.fPos, level, worth);
    generateItems(killed.fPos, killed.itemDrops());
}

void eServerArea::removePlannedUnits() {
    for(const uint32_t charId : mUnitsToRemove) {
        removeUnit(charId);
    }
    mUnitsToRemove.clear();
}

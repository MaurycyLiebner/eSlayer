#include "eserverarea.h"

#include "actions/eunitbaseaction.h"
#include "actions/efolloweraction.h"
#include "actions/enpcaction.h"

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

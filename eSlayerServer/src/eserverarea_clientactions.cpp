#include "eserverarea.h"

#include "eitemgenerator.h"

#include <eSlayerHelpers/eunitsinfo.h>
#include <eSlayerHelpers/edifficulties.h>
#include <eSlayerHelpers/eobjectsinfo.h>
#include <eSlayerHelpers/esellers.h>
#include <eSlayerHelpers/emercenaries.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/eportals.h>
#include <eSlayerHelpers/eitemsdata.h>

#include <eSlayerMapGenerator/emap.h>

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
    if(u->fHealth <= 0) return;
    const auto tu = unit(unitId);
    if(!tu) return;
    if(tu->fHealth <= 0) return;
    const auto p = u->takePotion(itemId);
    tu->consumePotion(p);
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

bool eServerArea::resetSkillStats(
    const uint32_t clientId, const uint8_t npcId) {
    const auto u = unit(clientId);
    if(!u) return false;
    if(npcId < 0) return false;
    const auto& npc = eUnitsInfo::sUnits.get(npcId);
    if(!npc.fResetSkillStats) return false;
    u->resetSkillStats();
    u->setAttributesChanged(true);
    return true;
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
    if(&from != &to) {
        from.clientMoved(clientId);
        for(const auto& f : followers) {
            const auto uid = f->fCharId;
            from.removeUnit(uid);
        }
    }
    const bool r = to.addClient(
        clientId, u, followers,
        clientData, moveData, spawnPos);
    if(!r) return false;
    if(moveData.fType == eMoveToMapType::portal) {
        goThroughPortal(clientId, moveData.fPortalId);
    }
    return true;
}
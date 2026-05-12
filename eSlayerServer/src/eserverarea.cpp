#include "eserverarea.h"

#include "actions/eclientaction.h"
#include "actions/eunitbaseaction.h"
#include "actions/efolloweraction.h"

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

eTeamId eServerArea::sNextTeamId = eTeamId::playerTeam0;

eServerArea::eServerArea() :
    mMIncrementer(mUnitAreas),
    mNIncrementer(mUnitAreas) {
    iniMissileInc();
    iniNovaInc();
}

void eServerArea::iniMissileInc() {
    const auto obsticle = [this](const ePointF& pos) {
        return mMap->obsticle(pos);
    };

    const auto removeMissile = [this](const eMissile& m) {
        mMissiles.remove(m.fId);
    };

    const auto getUnit = [this](const int charId) {
        const auto u = mUnits.get(charId);
        return static_cast<eUnitData*>(u.get());
    };

    const auto hitAction = [this](const eMissile& m, eUnitData& u) {
        const auto& sm = static_cast<const eServerMissile&>(m);
        auto& su = static_cast<eServerUnit&>(u);
        if(sm.fHitAction) sm.fHitAction(su);
    };

    mMIncrementer.initialize(obsticle,
                             removeMissile,
                             getUnit,
                             hitAction);
}

void eServerArea::iniNovaInc() {
    const auto inside = [this](const int x, const int y) {
        return mMap->inside(x, y);
    };

    const auto getObjects = [this](const int x, const int y)
        -> const std::vector<int>& {
        return mMap->objects(x, y);
    };

    const auto getObject = [this](const int id) {
        return mMap->object(id);
    };

    const auto getTile = [this](const int x, const int y)
        -> const eTile& {
        return mMap->tile(x, y);
    };

    const auto removeNova = [this](const eNova& m) {
        mNovas.remove(m.fId);
    };

    const auto getUnit = [this](const int charId) {
        const auto u = mUnits.get(charId);
        return static_cast<eUnitData*>(u.get());
    };

    const auto hitAction = [this](const eNova& n, eUnitData& u) {
        const auto& sn = static_cast<const eServerNova&>(n);
        auto& su = static_cast<eServerUnit&>(u);
        if(sn.fHitAction) sn.fHitAction(su);
    };

    mNIncrementer.initialize(inside,
                             getObjects,
                             getObject,
                             getTile,
                             removeNova,
                             getUnit,
                             hitAction);
}

void eServerArea::iniSetupUnit(
        const std::shared_ptr<eServerUnit>& u,
        const int charId,
        const eTeamId teamId,
        const ePointF& pos,
        const eUnitInfo& uinfo,
        const eCharData& data,
        const eModelParts& modelParts) {
    u->fCharId = charId;
    u->fTeamId = teamId;
    u->fState = 0;
    u->fCharDataId = uinfo.fCharData;
    u->fRadius = uinfo.fRadius;
    u->fPos = pos;
    u->fAnim = data.animId("stand");
    u->fAnimId = 0;
    u->fAnimSpeed = 1.f;
    u->fAngle = eRand::randF(0.f, 360.f);
    u->fBlockingActionTime = 0.f;
    u->fModelParts = modelParts;

    auto& m = u->movementHandler();
    m.setSpeed(uinfo.fWalkSpeed);
    const auto w = [this](const ePointF& pos) {
        return walkable(pos);
    };
    const auto iter = [this, charId](
        const ePointF& pos,
        const float dist,
        const eOtherHandler& handler) {
        iterateOverUnits(pos, dist, [handler, charId](
            const std::shared_ptr<eServerUnit>& u) {
            if(charId == u->fCharId) return false;
            handler(*u);
            return false;
        });
    };
    m.intialize(w, iter, charId, teamId);
    m.setRadius(u->fRadius);

    mUnits.add(charId, u);
    const auto area = unitArea(*u);
    mUnitAreas.emplace(area, charId);
}

void eServerArea::addGroundItem(
    const ePointF& pos, const eItem& item) {
    const auto itemId = item.fItemId;
    const auto groundItem = std::make_shared<eGroundItem>();
    groundItem->fItemId = itemId;
    groundItem->fDataId = item.fDataId;
    groundItem->fType = item.fType;
    groundItem->fSubType = item.fSubType;
    groundItem->fRarity = item.fRarity;
    groundItem->fSockets = item.fSockets;
    const auto baseTile = mItemTiles.posArea(pos);
    bool found = false;
    for(int dist = 0; dist < 100; dist++) {
        for(int x = dist; x >= -dist; x--) {
            for(int y = dist; y >= -dist; y--) {
                if(std::abs(x) != dist && std::abs(y) != dist) continue;
                const eArea tile{baseTile.fX + x, baseTile.fY + y};

                const auto tilePos = mItemTiles.areaPos(tile);
                const bool w = walkable(tilePos);
                if(!w) continue;

                if(!mItemTiles.hasArea(tile)) continue;
                const auto& items = mItemTiles.at(tile);
                if(!items.empty()) continue;
                groundItem->fPos = tilePos;
                mItemTiles.emplace(tile, itemId);
                found = true;
                break;
            }
            if(found) break;
        }
        if(found) break;
    }
    if(!found) return;
    mGroundItems.add(itemId, groundItem);
    mItemsOnGround.add(itemId, std::make_shared<eItem>(item));
    const auto area = itemArea(itemId);
    mItemAreas.emplace(area, itemId);
}

void eServerArea::generateItems(
    const ePointF& pos, const float level,
    const float worth) {
    float remWorth = worth;
    while(remWorth >= 0.25f) {
        const float worth = eRand::randF(0.25f, remWorth);
        generateItem(pos, level, worth);
        remWorth -= worth;
    }
}

void eServerArea::generateItem(
    const ePointF& pos, const float level,
    const float worth) {
    const auto item = eItemGenerator::generateItem(level, worth);
    addGroundItem(pos, item);
}

void eServerArea::generatePotion(
    const ePointF& pos, const float level,
    const float worth) {
    const auto item = eItemGenerator::generatePotion(level, worth);
    addGroundItem(pos, item);
}

void eServerArea::initialize(const std::shared_ptr<eMap>& map) {
    mMap = map;

    const int w = map->width();
    const int h = map->height();
    mUnitAreas.initialize(w, h, mUnitAreaDim);
    mItemAreas.initialize(w, h, mItemAreaDim);
    mItemTiles.initialize(w, h, -mItemTileSubdivision);

    const auto& mareas = map->monsterAreas();
    for(const auto& marea : mareas) {
        const auto& rect = marea.fRect;
        const auto& ms = marea.fSettings;
        const auto& types = ms.fTypes;
        if(types.empty()) continue;

        const int margin = 4;

        const auto tryAddUnits = [&](const int x, const int y) {
            for(int dx = -margin; dx <= margin; dx++) {
                const int xx = x + dx;
                if(xx < 0) return;
                if(xx >= w) return;
                for(int dy = -margin; dy <= margin; dy++) {
                    const int yy = y + dy;
                    if(yy < 0) return;
                    if(yy >= h) return;
                    const auto uarea = mUnitAreas.posArea(ePoint{xx, yy});
                    if(!mUnitAreas.hasArea(uarea)) continue;
                    const auto& us = mUnitAreas.at(uarea);
                    if(!us.empty()) return;
                }
            }

            const int nTypes = types.size();
            const int startId = eRand::rand() % nTypes;
            for(int i = 0; i < nTypes; i++) {
                const int typeId = (startId + i) % nTypes;
                const auto& typeData = types[typeId];
                const auto type = typeData.fType;
                const bool add = eRand::randChance(typeData.fProbability);
                if(!add) continue;

                const bool elite = eRand::randChance(typeData.fEliteProbability);
                bool boss = elite;
                eEliteModifiers mods;
                if(elite) {
                    mods.initialize(1, 1.f);
                }

                const auto& udata = eUnitsInfo::sUnits.get(type);
                const auto& data = eCharDataInfo::get(udata.fCharData);

                const auto addUnit = [&]() {
                    const auto modelParts = data.randomModelParts();
                    const auto u = std::make_shared<eServerUnit>(
                        false, data, type, *this);
                    const int charId = eServerUnit::sNextCharId++;
                    ePointF pos;
                    for(int dist = 1; dist < 3; dist++) {
                        const int maxTries = 10;
                        for(int i = 0; i <= maxTries; i++) {
                            if(i == maxTries) return;
                            const float dx = eRand::randF(-dist, dist);
                            const float dy = eRand::randF(-dist, dist);
                            const ePointF tryPos{float(x + dx), float(y + dy)};
                            const auto u = unit(tryPos);
                            if(u) continue;
                            const bool w = walkable(tryPos);
                            if(!w) continue;
                            pos = tryPos;
                            break;
                        }
                    }
                    iniSetupUnit(u, charId, eTeamId::neutralHostile,
                                 pos, udata, data, modelParts);

                    u->setBoosts(udata.fModifiers, false);

                    if(elite) {
                        mods.apply(*u, boss);
                        boss = false;
                    }

                    {
                        const int schoice = u->addSkill();
                        u->setSkillId(schoice, 0, false);
                    }
                    eSkillLevels skillLevels;
                    using sMap = std::map<uint16_t, uint16_t>;
                    reinterpret_cast<sMap&>(skillLevels) = udata.fSkills;
                    u->setSkillLevels(skillLevels, false);
                    for(const auto it : udata.fSkills) {
                        const int skillId = it.first;
                        const int schoice = u->addSkill();
                        u->setSkillId(schoice, skillId, false);
                    }
                    u->recalculateStats();

                    const auto a = std::make_shared<eUnitBaseAction>(*u, *this);
                    u->setAction(a);
                };

                const int nUnits = typeData.fGroupSize;
                for(int i = 0; i < nUnits; i++) {
                    addUnit();
                }
            }
        };

        for(int x = rect.fX + margin; x < rect.fX + rect.fW - margin; x++) {
            for(int y = rect.fY + margin; y < rect.fY + rect.fH - margin; y++) {
                tryAddUnits(x, y);
            }
        }
    }
}

void eServerArea::increment(const float by) {
    std::set<eArea> unitAreas;
    for(auto& it : mClientData) {
        const int i = it.first;
        const auto u = unit(i);
        if(!u) continue;
        auto& clientData = it.second;
        auto& oldArea = clientData.fArea;
        const auto newArea = unitArea(*u);
        if(oldArea != newArea) {
            mUnitAreas.erase(oldArea, i);
            mUnitAreas.emplace(newArea, i);
            oldArea = newArea;
        }

        const auto& screenDims = clientData.fScreen;
        const int halfHeight = std::ceil(0.5f*screenDims.fHeight/mUnitAreaDim);
        const int halfWidth = std::ceil(0.5f*screenDims.fWidth/mUnitAreaDim);
        const int dyMin = -halfHeight - 2;
        const int dyMax = halfHeight + 2;
        const int dxMin = -halfWidth - 2;
        const int dxMax = halfWidth + 2;
        for(int dy = dyMin; dy <= dyMax; dy++) {
            for(int dx = dxMin; dx <= dxMax; dx++) {
                const int y = newArea.fY - dx + dy/2;
                const int x = newArea.fX + dx + dy % 2 + dy/2;
                const eArea area{x, y};
                unitAreas.emplace(area);
            }
        }

        auto& followers = u->followers();
        for(int i = 0; i < followers.size(); i++) {
            const int charId = followers[i];
            const auto u = unit(charId);
            if(!u || u->fMaxHealth <= 0) {
                followers.erase(followers.begin() + i);
                i--;
            } else {
                const auto area = unitArea(*u);
                unitAreas.emplace(area);
            }
        }
    }

    for(const auto& area : unitAreas) {
        if(!mUnitAreas.hasArea(area)) continue;
        const auto units = mUnitAreas.at(area);
        for(const int charId : units) {
            const auto u = mUnits.get(charId);
            const auto oldArea = unitArea(*u);
            u->increment(by);
            const auto newArea = unitArea(*u);
            if(oldArea != newArea) {
                mUnitAreas.erase(oldArea, charId);
                mUnitAreas.emplace(newArea, charId);
            }
        }
    }

    for(const auto& m : mMissiles) {
        mMIncrementer.increment(*m, by);
    }

    for(const auto& n : mNovas) {
        mNIncrementer.increment(*n, by);
    }

    removePlannedUnits();

    mTime += by;
}

void eServerArea::unitsData(
    const int clientId,
    std::vector<eUnitData>& newUnits,
    std::vector<eUnitDynamicData>& updatedUnits) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return;
    auto& clientData = it->second;
    auto& known = clientData.fKnownUnits;
    std::set<int> visible;
    const auto& clientArea = clientData.fArea;
    const auto& screenDims = clientData.fScreen;
    const int halfHeight = std::ceil(0.5f*screenDims.fHeight/mUnitAreaDim);
    const int halfWidth = std::ceil(0.5f*screenDims.fWidth/mUnitAreaDim);
    const int dyMin = -halfHeight - 2;
    const int dyMax = halfHeight + 2;
    const int dxMin = -halfWidth - 2;
    const int dxMax = halfWidth + 2;
    for(int dy = dyMin; dy <= dyMax; dy++) {
        for(int dx = dxMin; dx <= dxMax; dx++) {
            const int y = clientArea.fY - dx + dy/2;
            const int x = clientArea.fX + dx + dy % 2 + dy/2;
            const eArea area{x, y};
            if(!mUnitAreas.hasArea(area)) continue;
            const auto& units = mUnitAreas.at(area);
            for(const int charId : units) {
                const auto u = unit(charId);
                if(!u) continue;
                visible.emplace(charId);
                if(known.find(charId) == known.end()) {
                    newUnits.emplace_back(u->toUnitData());
                    known.emplace(charId);
                } else {
                    updatedUnits.emplace_back(u->toDynamicData());
                }
            }
        }
    }
    // Remove units no longer visible from the known set
    for(auto it = known.begin(); it != known.end(); ) {
        if(visible.find(*it) == visible.end()) {
            it = known.erase(it);
        } else {
            ++it;
        }
    }
}

void eServerArea::itemsData(
    const int clientId,
    std::vector<eGroundItem>& newItems,
    std::vector<uint32_t>& removedItemIds) {
    const auto client = unit(clientId);
    if(!client) return;
    auto& clientData = mClientData[clientId];
    auto& known = clientData.fKnownItems;
    std::set<int> visible;
    const auto& clientPos = client->fPos;
    const auto clientArea = mItemAreas.posArea(clientPos);
    const auto& screenDims = clientData.fScreen;
    const int halfHeight = std::ceil(0.5f*screenDims.fHeight/mItemAreaDim);
    const int halfWidth = std::ceil(0.5f*screenDims.fWidth/mItemAreaDim);
    const int dyMin = -halfHeight - 1;
    const int dyMax = halfHeight + 1;
    const int dxMin = -halfWidth - 1;
    const int dxMax = halfWidth + 1;
    for(int dy = dyMin; dy <= dyMax; dy++) {
        for(int dx = dxMin; dx <= dxMax; dx++) {
            const int y = clientArea.fY - dx + dy/2;
            const int x = clientArea.fX + dx + dy % 2 + dy/2;
            const eArea area{x, y};
            if(!mItemAreas.hasArea(area)) continue;
            const auto& items = mItemAreas.at(area);
            for(const int itemId : items) {
                const auto i = groundItem(itemId);
                if(!i) continue;
                visible.emplace(itemId);
                if(known.find(itemId) == known.end()) {
                    newItems.emplace_back(*i);
                    known.emplace(itemId);
                }
            }
        }
    }
    for(auto it = known.begin(); it != known.end(); ) {
        if(visible.find(*it) == visible.end()) {
            removedItemIds.emplace_back(*it);
            it = known.erase(it);
        } else {
            ++it;
        }
    }
}

eArea eServerArea::unitArea(const int charId) const {
    const auto u = unit(charId);
    if(!u) return {0, 0};
    return unitArea(*u);
}

eArea eServerArea::unitArea(const eServerUnit& u) const {
    const auto& pos = u.fPos;
    return mUnitAreas.posArea(pos);
}

eArea eServerArea::itemArea(const int itemId) const {
    const auto i = groundItem(itemId);
    if(!i) return {0, 0};
    return itemArea(*i);
}

eArea eServerArea::itemArea(const eGroundItem& i) const {
    const auto& pos = i.fPos;
    return mItemAreas.posArea(pos);
}

eArea eServerArea::itemTile(const int itemId) const {
    const auto i = groundItem(itemId);
    if(!i) return {0, 0};
    return itemTile(*i);
}

eArea eServerArea::itemTile(const eGroundItem& i) const {
    const auto& pos = i.fPos;
    return mItemTiles.posArea(pos);
}

bool eServerArea::mapPortions(
    const int clientId,
    std::vector<eMapPortion>& result) {
    const auto u = unit(clientId);
    if(!u) return false;
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    auto& known = clientData.fKnownMap;
    const bool addAll = known.empty();
    const auto area = known.posArea(u->fPos);
    const int m = 1;
    for(int x = area.fX - m; x <= area.fX + m; x++) {
        for(int y = area.fY - m; y <= area.fY + m; y++) {
            const eArea xyArea{x, y};
            const bool r = known.hasArea(xyArea);
            if(!r) {
                const auto pos = known.areaPos(xyArea).round();
                const eMapPortionArea mapArea{pos.fX,
                                              pos.fY,
                                              eMapPortion::sBaseDim,
                                              eMapPortion::sBaseDim};
                eMapPortion p;
                const bool r = mMap->extractPortion(mapArea, p);
                if(r) {
                    result.emplace_back(std::move(p));
                    known.emplace(xyArea);
                    if(!addAll) return true;
                }
            }
        }
    }
    return addAll;
}

bool eServerArea::walkable(const ePointF& pos) const {
    return mMap->walkable(pos);
}

bool eServerArea::addClient(const int clientId,
                            eCharacter& c,
                            eTeamId& teamId,
                            const eScreenDimensions& screenDims) {
    const int typeId = 0;
    const auto& udata = eUnitsInfo::sUnits.get(typeId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    const std::map<std::string, std::string> partsMap{{"whole", "light"}};
    const auto modelParts = data.mapToModelParts(partsMap);
    const auto u = std::make_shared<eServerUnit>(
        true, data, typeId, *this);
    u->addSkill();
    u->addSkill();
    const auto& spawnPos = mMap->spawnPos();
    teamId = sNextTeamId;
    iniSetupUnit(u, clientId, teamId, spawnPos, udata, data, modelParts);
    sNextTeamId = static_cast<eTeamId>(static_cast<int>(sNextTeamId) + 1);
    const auto a = std::make_shared<eClientAction>(*u, *this);
    u->setAction(a);
    auto& eq = c.equipment();
    eq.iterateOverAll([](eItem& item) {
        eItemGenerator::applyItemId(item);
    });
    u->setEquipment(eq, false);
    const auto& attrs = c.attributes();
    u->setAttributes(attrs, false);
    const auto& skillLevels = c.skillLevels();
    u->setSkillLevels(skillLevels, false);
    u->recalculateStats();

    auto& clientData = mClientData[clientId];
    clientData.fLatestMissile = 0;
    clientData.fLatestNova = 0;
    clientData.fScreen = screenDims;
    const auto area = unitArea(*u);
    clientData.fArea = area;

    return true;
}

bool eServerArea::respawn(const int clientId) {
    const auto client = unit(clientId);
    if(!client) return false;
    const bool createBody = true;
    if(createBody) {
        auto& eq = client->equipment();
        const auto& data = client->data();
        const int typeId = 0;
        const auto u = std::make_shared<eServerUnit>(
            true, data, typeId, *this);
        u->setEquipment(eq, false);
        const auto& udata = eUnitsInfo::sUnits.get(typeId);
        const int charId = eServerUnit::sNextCharId++;
        const auto& modelParts = client->fModelParts;
        const auto teamId = client->fTeamId;
        const auto& pos = client->fPos;
        iniSetupUnit(u, charId, teamId, pos, udata, data, modelParts);
        u->fHealth = 0;
        u->fAnim = data.animId("body");
        client->setEquipment(eEquipment());
        {
            const auto it = mClientData.find(clientId);
            if(it != mClientData.end()) {
                auto& client = it->second;
                client.fBodies.emplace_back(charId);
            }
        }
    }
    client->respawn();
    client->fPos = mMap->spawnPos();
    return true;
}

bool eServerArea::removeClient(const int clientId) {
    planRemoveUnit(clientId);
    const int r = mClientData.erase(clientId);
    return r > 0;
}

bool eServerArea::planRemoveUnit(const int charId) {
    mUnitsToRemove.emplace_back(charId);
    return true;
}

bool eServerArea::pickupBody(
    const int clientId, const int charId) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& client = it->second;
    auto& bodies = client.fBodies;
    const auto bit = std::find(bodies.begin(), bodies.end(), charId);
    if(bit == bodies.end()) return false;
    const auto body = unit(charId);
    if(!body) return false;
    const auto u = unit(clientId);
    if(!u || u->fHealth <= 0) return false;
    const float dist = ePointF::distance(body->fPos, u->fPos);
    if(dist > 1.f) return false;
    auto& dst = u->equipment();
    auto& src = body->equipment();
    dst.moveFrom(src);
    if(src.empty()) {
        bodies.erase(bit);
        planRemoveUnit(charId);
    }
    u->recalculateStats();
    return true;
}

std::shared_ptr<eObject> eServerArea::triggerObject(
    const int clientId, const int objectId,
    const int tx, const int ty) {
    const auto& objIds = mMap->objects(tx, ty);
    for(const auto id : objIds) {
        const auto& obj = mMap->object(id);
        const auto objId = obj->fObjectId;
        if(objId != objectId) continue;
        const auto type = obj->fObjectType;
        const auto& info = eObjectsInfo::sObjects.get(type);
        switch(info.fType) {
        case eObjectType::treasure: {
            auto& state = obj->fState;
            if(state != 0) return nullptr;
            const float fx = tx + obj->fSize + 0.5f;
            const ePointF pos{fx, float(ty)};
            generateItems(pos, 5.f, 7.5f);
            state = 1;
        } break;
        case eObjectType::none:
            break;
        }
        return obj;
    }
    return nullptr;
}

bool eServerArea::triggerDoors(
    const int clientId, const eDoors& doors) {
    const bool r = mMap->inside(doors.fX, doors.fY);
    if(!r) return false;
    auto& tile = mMap->tile(doors.fX, doors.fY);
    switch(doors.fType) {
    case eWallType::topLeft: {
        eTile::setOpen(tile.fWallTL, !doors.fOpen);
        return true;
    } break;
    case eWallType::topRight: {
        eTile::setOpen(tile.fWallTR, !doors.fOpen);
        return true;
    } break;
    }
    return false;
}

bool eServerArea::pickupItem(
    const int clientId, const int itemId,
    const bool drag) {
    const auto u = unit(clientId);
    if(!u) return false;
    const auto item = mItemsOnGround.get(itemId);
    if(!item) return false;
    const auto gitem = mGroundItems.get(itemId);
    const auto area = itemArea(itemId);
    const auto tile = itemTile(itemId);
    auto& eq = u->equipment();
    if(drag) {
        if(eq.fDragged.fType != eItemType::none) return false;
        eq.fDragged = *item;
    } else {
        const auto& stats = u->stats();
        const bool met = stats.itemReqsMet(*item);
        const bool r = eq.add(*item, met);
        if(!r) return false;
        u->recalculateStats();
    }
    mGroundItems.remove(itemId);
    mItemsOnGround.remove(itemId);
    mItemAreas.erase(area, itemId);
    mItemTiles.erase(tile, itemId);
    return true;
}

bool eServerArea::dropItem(const int clientId) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    const auto pos = u->fPos;
    auto& item = eq.fDragged;
    if(item.fType == eItemType::none) return false;
    addGroundItem(pos, item);
    item = eItem();
    return true;
}

void eServerArea::rearrangeItems(
    const int clientId, const eEquipment& eq) {
    const auto u = unit(clientId);
    if(!u) return;
    u->setEquipment(eq);
}

void eServerArea::changeAttributes(
    const int clientId, const eAttributes& attrs) {
    const auto u = unit(clientId);
    if(!u) return;
    u->setAttributes(attrs);
}

void eServerArea::changeSkillLevels(
    const int clientId, const eSkillLevels& skillLevels) {
    const auto u = unit(clientId);
    if(!u) return;
    u->setSkillLevels(skillLevels);
}

void eServerArea::consumePotion(
    const int clientId, const uint32_t itemId) {
    const auto u = unit(clientId);
    if(!u) return;
    u->consumePotion(itemId);
}

std::vector<eMissile>
eServerArea::missileData(const int clientId) {
    std::vector<eMissile> result;
    const auto u = unit(clientId);
    if(!u) return result;
    result.reserve(mMissiles.actualSize());
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return result;
    auto& clientData = it->second;
    auto& latestMissile = clientData.fLatestMissile;
    auto newLatestMissile = latestMissile;
    for(const auto& m : mMissiles) {
        if(m->fId <= latestMissile) continue;
        newLatestMissile = std::max(newLatestMissile, m->fId);
        const float dist = ePointF::distance(m->fPos, u->fPos);
        if(dist > 20.f) continue;
        result.emplace_back(*m);
    }
    latestMissile = newLatestMissile;
    return result;
}

std::vector<eNova>
eServerArea::novaData(const int clientId) {
    std::vector<eNova> result;
    const auto u = unit(clientId);
    if(!u) return result;
    result.reserve(mNovas.actualSize());
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return result;
    auto& clientData = it->second;
    auto& latestNova = clientData.fLatestNova;
    auto newLatestNova = latestNova;
    for(const auto& n : mNovas) {
        if(n->fId <= latestNova) continue;
        newLatestNova = std::max(newLatestNova, n->fId);
        const float dist = ePointF::distance(n->fCenter, u->fPos);
        if(dist > 20.f) continue;
        result.emplace_back(*n);
    }
    latestNova = newLatestNova;
    return result;
}

std::vector<int>
eServerArea::bodies(const int clientId) {
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return {};
    auto& clientData = it->second;
    std::vector<int> result;
    const auto& bodies = clientData.fBodies;
    const int known = clientData.fKnownBodies;
    for(int i = known; i < bodies.size(); i++) {
        result.emplace_back(bodies[i]);
    }
    return result;
}

void eServerArea::addMissile(const std::shared_ptr<eServerMissile>& m) {
    mMissiles.add(m->fId, m);
}

void eServerArea::addNova(const std::shared_ptr<eServerNova>& n) {
    mNovas.add(n->fId, n);
}

int piercedFromPierceChance(const float p) {
    const float u = eRand::randF();
    return int(std::log(u) / std::log(p));
}

void eServerArea::spawnMissile(const ePointF& to,
                               const eSkill& skill,
                               const eHitData& data,
                               const int nMissiles,
                               const float pierceChance,
                               const int missileId,
                               const float missileRangeTime,
                               const bool continuousDamage) {
    const auto skillType = skill.fType;
    auto baseDir = ePointF::vector(to, data.fFrom);
    if(baseDir.length() < 0.001f) baseDir = eVec2f::random();
    struct eMissileData {
        ePointF fPos;
        ePointF fTo;
        int fToPierce;
        int fMissileId;
        float fRangeTime;
        eDamage fDamage;
    };
    std::vector<eMissileData> missiles;
    const auto spawnMissiles = [&](const int missileId,
                                   const float missileRangeTime) {
        float maxAngle = skill.fMaxAngle;
        if(skill.fAngleAdjust) {
            if(skill.fRangeTime > 0.f) {
                const float len = baseDir.length();
                const float multBase = 1.f - 3.f*len/skill.fRangeTime;
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
            md.fToPierce = 1 + std::min(max, pierced);
            md.fPos = data.fFrom;
            md.fTo = data.fFrom + dir;
            md.fMissileId = missileId;
            md.fRangeTime = missileRangeTime;
            md.fDamage = data.fDamage;
            if(nMissiles > 1) {
                angle += maxAngle/(nMissiles - 1);
            }
        }
    };
    if(skillType == eSkillType::missile) {
        spawnMissiles(missileId, missileRangeTime);
    } else if(skillType == eSkillType::wall) {
        eVec2f perp(-baseDir.y, baseDir.x);
        perp.normalize(2*skill.fRadius);
        ePointF pt = to - perp * (nMissiles/2);
        for(int i = 0; i < nMissiles; i++) {
            auto& md = missiles.emplace_back();
            md.fToPierce = 0;
            md.fPos = pt;
            md.fTo = pt;
            md.fMissileId = skill.fMissileId;
            md.fRangeTime = skill.fRangeTime;
            md.fDamage = data.fDamage;
            pt = pt + perp;
        }
    } else {
        spawnMissiles(missileId, missileRangeTime);
    }
    for(const auto& md : missiles) {
        const auto m = std::make_shared<eServerMissile>();
        m->fType = md.fMissileId;
        m->fTeamId = data.fAttackTeamId;
        m->fToPierce = md.fToPierce;
        m->fSpeed = skill.fSpeed;
        m->fRemDistTime = md.fRangeTime;
        m->fPathType = skill.fPathId;
        m->fFrom = data.fFrom;
        m->fRadius = skill.fRadius;
        m->fPos = md.fPos;
        m->fTo = md.fTo;
        m->fContinuousDamage = continuousDamage;
        m->fEnemyFindRange = skill.fMissileEnemyFindRange;
        m->fTime = 0.f;
        struct eCharSkipper {
            float fTime = 0.f;
            std::set<int> fChars;
        };

        const std::shared_ptr<eCharSkipper> skip =
            continuousDamage ?
                std::make_shared<eCharSkipper>() :
                nullptr;
        m->fHitAction = [data, m, skip](eServerUnit& u) {
            if(skip) {
                auto& c = skip->fChars;
                if(skip->fTime < m->fTime) {
                    c.clear();
                    skip->fTime = m->fTime;
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
}

void eServerArea::spawnNova(const eSkill& skill,
                            const eHitData& data,
                            const bool continuousDamage) {
    const auto n = std::make_shared<eServerNova>();
    n->fTeamId = data.fAttackTeamId;
    n->fMissileType = skill.fMissileId;
    n->fCenter = data.fFrom;
    n->fRadius = 0.f;
    n->fMaxRadius = skill.fRadius;
    n->fSpeed = skill.fSpeed;

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

void eServerArea::summon(eServerUnit& by,
                         ePointF to,
                         const int unitId,
                         const int maxCount,
                         const std::vector<eModifier>& mods) {
    auto& followers = by.followers();
    const auto summoned = eServerArea::summoned(by, unitId);
    if(maxCount > 0 && summoned.size() >= maxCount) {
        const int removeCharId = summoned[0];
        planRemoveUnit(removeCharId);
        eVectorHelpers::remove(followers, removeCharId);
    }
    to = emptyPlaceNear(to);
    const auto& udata = eUnitsInfo::sUnits.get(unitId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    const auto name = data.name();
    std::map<std::string, std::string> partsMap = {
        {"wolf", "whole"}
    };
    const auto modelParts = data.mapToModelParts(partsMap);
    const auto u = std::make_shared<eServerUnit>(
        false, data, unitId, *this);
    const int charId = eServerUnit::sNextCharId++;
    followers.emplace_back(charId);
    iniSetupUnit(u, charId, by.fTeamId, to, udata, data, modelParts);
    u->setBoosts(mods, false);
    {
        const int schoice = u->addSkill();
        u->setSkillId(schoice, 0, false);
    }
    u->recalculateStats();

    const auto byPtr = unit(by.fCharId);
    const auto a = std::make_shared<eFollowerAction>(*u, *this, byPtr);
    u->setAction(a);
}

void eServerArea::castChance(eServerUnit& by,
                       const eSkillStats& o,
                       const eWeaponChoice wchoice,
                       const ePointF& to) {
    const bool r = eRand::randChance(o.fCastChance);
    if(!r) return;
    return cast(by, o, wchoice, to);
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
        const float missileRangeTime = by.missileRangeTime(o, wchoice);
        const bool continuousDamage = skill.fType == eSkillType::wall;
        spawnMissile(to, skill, data,
                     nMissiles, pierceChance, missileId,
                     missileRangeTime, continuousDamage);
    } break;
    case eSkillType::nova: {
        const bool continuousDamage = false;
        spawnNova(skill, data, continuousDamage);
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
    case eSkillType::boostCurse: {

    } break;
    case eSkillType::attack:
    case eSkillType::aura:
    case eSkillType::shoot:
    case eSkillType::kick:
    case eSkillType::smite:
    case eSkillType::passive:
    case eSkillType::throw_:
        break;
    }
}

std::vector<int> eServerArea::summoned(
    const eServerUnit& by, const int unitId) {
    std::vector<int> result;
    const auto& followers = by.followers();
    for(const auto charId : followers) {
        const auto u = unit(charId);
        const auto unitIdU = u->unitTypeId();
        if(unitIdU == unitId) {
            result.emplace_back(charId);
        }
    }
    return result;
}

ePointF eServerArea::emptyPlaceNear(const ePointF& pos) const {
    for(int dist = 0; dist < 100; dist++) {
        for(int x = -dist; x <= dist; x++) {
            for(int y = -dist; y <= dist; y++) {
                if(std::abs(x) != dist && std::abs(y) != dist) continue;
                const ePointF p{pos.fX + x*0.5f, pos.fY + y*0.5f};
                const bool r = mMap->walkable(p);
                if(!r) continue;
                const auto u = unit(p, [](const eServerUnit& u) {
                    return u.fHealth > 0;
                });
                if(u) continue;
                return p;
            }
        }
    }
    return pos;
}

std::shared_ptr<eServerUnit>
eServerArea::unit(const int charId) const {
    return mUnits.get(charId);
}

std::shared_ptr<eGroundItem>
eServerArea::groundItem(const int itemId) const {
    return mGroundItems.get(itemId);
}

std::shared_ptr<eServerUnit> eServerArea::unit(
    const ePointF& pos, const eValidator& validator) const {
    std::shared_ptr<eServerUnit> result;

    const auto iter = [&](const std::shared_ptr<eServerUnit>& u) {
        if(validator) {
            const bool r = validator(*u);
            if(!r) return false;
        }
        const auto& upos = u->fPos;
        const float dist = ePointF::distance(pos, upos);
        if(dist <= u->fRadius) {
            result = u;
            return true;
        }
        return false;
    };

    const float maxRadius = 1.f;
    iterateOverUnits(pos, maxRadius, iter);
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
            for(const int charId : units) {
                const auto u = unit(charId);
                if(!u) continue;
                const bool r = iter(u);
                if(r) return true;
            }
        }
    }
    return false;
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
    const float level = killed.level();
    const auto type = killed.unitType();
    float worth = 0.f;
    switch(type) {
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

    if(worth > 0.f) generateItem(killed.fPos, level, worth);
    for(const auto& c : mClientData) {
        const int clientId = c.first;
        const auto u = unit(clientId);
        if(!u) continue;
        if(u->fHealth <= 0) continue;
        const eTeamId t1 = u->fTeamId;
        const eTeamId t2 = killed.fTeamId;
        if(!eTeams::areEnemies(t1, t2)) continue;
        const float dist = ePointF::distance(u->fPos, killed.fPos);
        if(dist > 10.f) continue;
        u->killed(killed);
    }
}

void eServerArea::removePlannedUnits() {
    for(const int charId : mUnitsToRemove) {
        const auto area = unitArea(charId);
        mUnitAreas.erase(area, charId);
        mUnits.remove(charId);
    }
    mUnitsToRemove.clear();
}

eClientData::eClientData() :
    fKnownMap(eMapPortion::sBaseDim) {}

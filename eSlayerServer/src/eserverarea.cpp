#include "eserverarea.h"

#include "../../eSlayerHelpers/include/eSlayerHelpers/echardatainfo.h"
#include "actions/eclientaction.h"
#include "actions/eunitbaseaction.h"
#include "actions/efolloweraction.h"

#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/evectorhelpers.h>
#include <eSlayerHelpers/echaracter.h>
#include <eSlayerHelpers/eunitsinfo.h>

#include <eSlayerMissiles/emissileincrementer.h>
#include <eSlayerMissiles/emissilecollision.h>
#include <eSlayerMissiles/emissileincrement.h>

uint32_t eServerArea::sNextItemId = 1;

eServerArea::eServerArea() :
    mUnitAreas(mUnitAreaDim),
    mItemAreas(mItemAreaDim),
    mItemTiles(-mItemTileSubdivision),
    mMIncrementer(mUnitAreas) {
    const auto obsticle = [this](const ePointF& pos) {
        const auto ipos = pos.floor();
        return !mMap->walkable(ipos.fX, ipos.fY);
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

void eServerArea::initialize(const std::shared_ptr<eMap>& map) {
    mMap = map;

    for(int x = 24; x < 75; x++) {
        if(x == 40) x += 20;
        for(int y = 24; y < 75; y++) {
            if(y == 40) y += 20;
            const int typeId = 1 + eRand::rand() % 2;
            const auto& udata = eUnitsInfo::sUnits.get(typeId);
            const auto& data = eCharDataInfo::get(udata.fCharData);
            const auto name = data.name();
            std::map<std::string, std::string> partsMap;
            if(name == "mummy") {
                partsMap = {
                    {"mummy", "whole"}
                };
            } else if(name == "wendigo") {
                partsMap = {
                    {"wendigo", "whole"}
                };
            } else {
                continue;
            }
            const auto modelParts = data.mapToModelParts(partsMap);
            const auto u = std::make_shared<eServerUnit>(false, data, *this);
            const int charId = eServerUnit::sNextCharId++;
            u->fCharId = charId;
            u->fTeamId = -1;
            u->fCharDataId = udata.fCharData;
            u->fModelParts = modelParts;
            u->fHealth = 100;
            u->fMaxHealth = 100;
            u->fRadius = udata.fRadius;
            u->fAnim = data.animId("stand");
            u->fAnimSpeed = 1.f;
            u->fBlockingActionTime = 0.f;
            u->fAnimId = 0;
            const ePointF pos{float(x), float(y)};
            u->fPos = pos;
            u->fAngle = 0.f;
            {
                const int schoice = u->addSkill();
                u->setSkillId(schoice, 0, false);
            }
            // {
            //     const int schoice = u->addSkill();
            //     const int skillId = eSkills::sSkills.id("fireball");
            //     u->setSkillId(schoice, skillId, false);
            // }
            u->recalculateStats();
            mUnits.add(charId, u);
            const auto area = unitArea(*u);
            mUnitAreas.emplace(area, charId);

            auto& m = u->movementHandler();
            m.setSpeed(udata.fWalkSpeed);

            const auto w = [this](const int x, const int y) {
                return mMap->walkable(x, y);
            };
            const auto iter = [this, charId](const eOtherHandler& handler) {
                for(const auto& u : mUnits) {
                    if(charId == u->fCharId) continue;
                    handler(*u);
                }
            };
            m.intialize(w, iter, charId, -1);
            m.setRadius(u->fRadius);
            m.setPos(pos);

            const auto a = std::make_shared<eUnitBaseAction>(*u, *this);
            u->setAction(a);
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

bool eServerArea::mapPortion(
    const int clientId, eMapPortion& result) {
    const auto u = unit(clientId);
    if(!u) return false;
    const auto it = mClientData.find(clientId);
    if(it == mClientData.end()) return false;
    auto& clientData = it->second;
    auto& known = clientData.fKnownMap;
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
                const bool r = mMap->extractPortion(mapArea, result);
                if(r) {
                    known.emplace(xyArea);
                    return true;
                }
            }
        }
    }
    return false;
}

bool eServerArea::addClient(const int clientId,
                            eCharacter& c,
                            const ePointF& pos,
                            const eScreenDimensions& screenDims) {
    auto& clientData = mClientData[clientId];
    clientData.fLatestMissile = -1;
    clientData.fScreen = screenDims;
    const int typeId = 0;
    const auto& udata = eUnitsInfo::sUnits.get(typeId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    const std::map<std::string, std::string> partsMap{{"whole", "light"}};
    const auto modelParts = data.mapToModelParts(partsMap);
    const auto u = std::make_shared<eServerUnit>(true, data, *this);
    u->addSkill();
    u->addSkill();
    u->fCharId = clientId;
    u->fCharDataId = udata.fCharData;
    u->fRadius = udata.fRadius;
    u->fAnim = data.animId("stand");
    u->fAnimId = 0;
    u->fAnimSpeed = 1.f;
    u->fTeamId = 0;
    u->fPos = pos;
    u->fAngle = 0.f;
    u->fMaxHealth = 100;
    u->fHealth = 100;
    u->fBlockingActionTime = 0.f;
    u->fModelParts = modelParts;
    const auto a = std::make_shared<eClientAction>(*u, *this);
    u->setAction(a);
    auto& eq = c.equipment();
    eq.iterateOverAll([](eItem& item) {
        item.fItemId = sNextItemId++;
    });
    u->setEquipment(eq, false);
    const auto& attrs = c.attributes();
    u->setAttributes(attrs, false);
    const auto& skillLevels = c.skillLevels();
    u->setSkillLevels(skillLevels, false);
    u->recalculateStats();
    mUnits.add(clientId, u);
    const auto area = unitArea(*u);
    mUnitAreas.emplace(area, clientId);
    clientData.fArea = area;

    auto& m = u->movementHandler();
    m.setSpeed(udata.fWalkSpeed);

    const auto w = [this](const int x, const int y) {
        return mMap->walkable(x, y);
    };
    const auto iter = [this, clientId](const eOtherHandler& handler) {
        for(const auto& u : mUnits) {
            if(clientId == u->fCharId) continue;
            handler(*u);
        }
    };
    m.intialize(w, iter, clientId, 0);
    m.setRadius(u->fRadius);
    m.setPos(pos);

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
        const bool r = eq.add(*item);
        if(!r) return false;
        u->recalculateStats();
    }
    mGroundItems.remove(itemId);
    mItemsOnGround.remove(itemId);
    mItemAreas.erase(area, itemId);
    mItemTiles.erase(tile, itemId);
    return true;
}

bool eServerArea::dropItem(const int clientId, const int itemId) {
    const auto u = unit(clientId);
    if(!u) return false;
    auto& eq = u->equipment();
    const auto pos = u->fPos;
    auto& item = eq.fDragged;
    if(item.fType == eItemType::none) return false;
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
        for(int x = -dist; x <= dist; x++) {
            for(int y = -dist; y <= dist; y++) {
                if(std::abs(x) != dist && std::abs(y) != dist) continue;
                const eArea tile{baseTile.fX + x, baseTile.fY + y};
                const auto& items = mItemTiles.at(tile);
                if(!items.empty()) continue;
                groundItem->fPos = ePointF{float(tile.fX)/mItemTileSubdivision,
                                           float(tile.fY)/mItemTileSubdivision};
                mItemTiles.emplace(tile, itemId);
                found = true;
                break;
            }
            if(found) break;
        }
        if(found) break;
    }
    mGroundItems.add(itemId, groundItem);
    mItemsOnGround.add(itemId, std::make_shared<eItem>(item));
    const auto area = itemArea(itemId);
    mItemAreas.emplace(area, itemId);
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

std::vector<eMissile>
eServerArea::missileData(const int clientId) {
    std::vector<eMissile> result;
    const auto u = unit(clientId);
    if(!u) return result;
    result.reserve(mMissiles.actualSize());
    auto& latestMissile = mClientData[clientId].fLatestMissile;
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

void eServerArea::addMissile(const std::shared_ptr<eServerMissile>& m) {
    mMissiles.add(m->fId, m);
}

void eServerArea::summon(eServerUnit& by,
                         ePointF to,
                         const int unitId,
                         const int maxCount) {
    auto& followers = by.followers();
    if(followers.size() >= maxCount && maxCount > 0) {
        const int removeCharId = followers[0];
        planRemoveUnit(removeCharId);
        followers.erase(followers.begin());
    }
    to = emptyPlaceNear(to);
    const auto& udata = eUnitsInfo::sUnits.get(unitId);
    const auto& data = eCharDataInfo::get(udata.fCharData);
    const auto name = data.name();
    std::map<std::string, std::string> partsMap = {
        {"wolf", "whole"}
    };
    const auto modelParts = data.mapToModelParts(partsMap);
    const auto u = std::make_shared<eServerUnit>(false, data, *this);
    const int charId = eServerUnit::sNextCharId++;
    followers.emplace_back(charId);
    u->fCharId = charId;
    u->fTeamId = by.fTeamId;
    u->fCharDataId = udata.fCharData;
    u->fModelParts = modelParts;
    u->fHealth = 100;
    u->fMaxHealth = 100;
    u->fRadius = udata.fRadius;
    u->fAnim = data.animId("stand");
    u->fAnimSpeed = 1.f;
    u->fBlockingActionTime = 0.f;
    u->fAnimId = 0;
    u->fPos = to;
    u->fAngle = 0.f;
    {
        const int schoice = u->addSkill();
        u->setSkillId(schoice, 0, false);
    }
    u->recalculateStats();
    mUnits.add(charId, u);
    const auto area = unitArea(*u);
    mUnitAreas.emplace(area, charId);

    auto& m = u->movementHandler();
    m.setSpeed(udata.fWalkSpeed);

    const auto w = [this](const int x, const int y) {
        return mMap->walkable(x, y);
    };
    const auto iter = [this, charId](const eOtherHandler& handler) {
        for(const auto& u : mUnits) {
            if(charId == u->fCharId) continue;
            handler(*u);
        }
    };
    m.intialize(w, iter, charId, by.fTeamId);
    m.setRadius(u->fRadius);
    m.setPos(to);

    const auto byPtr = unit(by.fCharId);
    const auto a = std::make_shared<eFollowerAction>(*u, *this, byPtr);
    u->setAction(a);
}

ePointF eServerArea::emptyPlaceNear(const ePointF& pos) const {
    for(int dist = 0; dist < 100; dist++) {
        for(int x = -dist; x <= dist; x++) {
            for(int y = -dist; y <= dist; y++) {
                if(std::abs(x) != dist && std::abs(y) != dist) continue;
                const ePointF p{pos.fX + x*0.5f, pos.fY + y*0.5f};
                const auto ipos = p.floor();
                const bool r = mMap->walkable(ipos.fX, ipos.fY);
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
    for(const auto& c : mClientData) {
        const int clientId = c.first;
        const auto u = unit(clientId);
        if(!u) continue;
        if(u->fHealth <= 0) continue;
        if(u->fTeamId == killed.fTeamId) continue;
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

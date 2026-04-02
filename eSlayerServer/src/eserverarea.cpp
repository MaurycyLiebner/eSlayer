#include "eserverarea.h"

#include "../../eSlayerHelpers/include/eSlayerHelpers/echardatainfo.h"
#include "eclientaction.h"
#include "eunitbaseaction.h"

#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/evectorhelpers.h>
#include <eSlayerHelpers/echaracter.h>

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
            const int typeId = 2 + eRand::rand() % 2;
            const auto& data = eCharDataInfo::get(typeId);
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
            const auto u = std::make_shared<eServerUnit>(data, *this);
            const int charId = eServerUnit::sNextCharId++;
            u->fCharId = charId;
            u->fTeamId = -1;
            u->fTypeId = typeId;
            u->fModelParts = modelParts;
            u->fHealth = 100;
            u->fMaxHealth = 100;
            u->fRadius = data.radius();
            u->fAnim = data.animId("stand");
            u->fActionTime = 0.f;
            u->fAnimId = 0;
            const ePointF pos{float(x), float(y)};
            u->fPos = pos;
            u->fAngle = 0.f;
            u->setSkillId(eSkillChoice::left, 0);
            mUnits.add(charId, u);
            const auto area = unitArea(*u);
            mUnitAreas.emplace(area, charId);

            auto& m = u->movementHandler();
            m.setSpeed(0.025f);

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
    std::set<eUnitArea> unitAreas;
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
                const eUnitArea area{x, y};
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
            const eUnitArea area{x, y};
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
            const eUnitArea area{x, y};
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

eUnitArea eServerArea::unitArea(const int charId) const {
    const auto u = unit(charId);
    if(!u) return {0, 0};
    return unitArea(*u);
}

eUnitArea eServerArea::unitArea(const eServerUnit& u) const {
    const auto& pos = u.fPos;
    return mUnitAreas.posArea(pos);
}

eUnitArea eServerArea::itemArea(const int itemId) const {
    const auto i = groundItem(itemId);
    if(!i) return {0, 0};
    return itemArea(*i);
}

eUnitArea eServerArea::itemArea(const eGroundItem& i) const {
    const auto& pos = i.fPos;
    return mItemAreas.posArea(pos);
}

eUnitArea eServerArea::itemTile(const int itemId) const {
    const auto i = groundItem(itemId);
    if(!i) return {0, 0};
    return itemTile(*i);
}

eUnitArea eServerArea::itemTile(const eGroundItem& i) const {
    const auto& pos = i.fPos;
    return mItemTiles.posArea(pos);
}

bool eServerArea::addClient(const int clientId,
                            eCharacter& c,
                            const ePointF& pos,
                            const eScreenDimensions& screenDims) {
    auto& clientData = mClientData[clientId];
    clientData.fLatestMissile = -1;
    clientData.fScreen = screenDims;
    const int typeId = 1;
    const auto& data = eCharDataInfo::get(typeId);
    const std::map<std::string, std::string> partsMap{{"whole", "light"}};
    const auto modelParts = data.mapToModelParts(partsMap);
    const auto u = std::make_shared<eServerUnit>(data, *this);
    u->fCharId = clientId;
    u->fTypeId = typeId;
    u->fRadius = data.radius();
    u->fAnim = data.animId("stand");
    u->fAnimId = 0;
    u->fAnimSpeed = 1.f;
    u->fTeamId = 0;
    u->fPos = pos;
    u->fAngle = 0.f;
    u->fMaxHealth = 100;
    u->fHealth = 100;
    u->fActionTime = 0.f;
    u->fModelParts = modelParts;
    const auto a = std::make_shared<eClientAction>(*u, *this);
    u->setAction(a);
    auto& eq = c.equipment();
    eq.iterateOverAll([](eItem& item) {
        item.fItemId = sNextItemId++;
    });
    u->setEquipment(eq);
    u->setAttributes(c.attributes());
    mUnits.add(clientId, u);
    const auto area = unitArea(*u);
    mUnitAreas.emplace(area, clientId);
    clientData.fArea = area;
    return true;
}

bool eServerArea::removeClient(const int clientId) {
    removeUnit(clientId);
    const int r = mClientData.erase(clientId);
    return r > 0;
}

bool eServerArea::removeUnit(const int charId) {
    const auto area = unitArea(charId);
    mUnitAreas.erase(area, charId);
    return mUnits.remove(charId);
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
                const eUnitArea tile{baseTile.fX + x, baseTile.fY + y};
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

std::shared_ptr<eServerUnit>
eServerArea::unit(const int charId) const {
    return mUnits.get(charId);
}

std::shared_ptr<eGroundItem>
eServerArea::groundItem(const int itemId) const {
    return mGroundItems.get(itemId);
}

std::shared_ptr<eServerUnit>
eServerArea::unit(const ePointF& pos) {
    const auto centralArea = mUnitAreas.posArea(pos);

    const float maxRadius = 1.f;
    const float minX = centralArea.fX - maxRadius;
    const float maxX = centralArea.fX + maxRadius;
    const float minY = centralArea.fY - maxRadius;
    const float maxY = centralArea.fY + maxRadius;

    const auto areaMin = mUnitAreas.posArea(ePointF{minX, minY});
    const auto areaMax = mUnitAreas.posArea(ePointF{maxX, maxY});

    for(int ax = areaMin.fX; ax <= areaMax.fX; ax++) {
        for(int ay = areaMin.fY; ay <= areaMax.fY; ay++) {
            const eUnitArea area{ax, ay};
            const auto& units = mUnitAreas.at(area);
            for(const int charId : units) {
                const auto u = unit(charId);
                if(!u) continue;
                const auto& upos = u->fPos;
                const float dist = ePointF::distance(pos, upos);
                if(dist <= u->fRadius) return u;
            }
        }
    }
    return nullptr;
}

void eServerArea::unitKilled(const eServerUnit& killed) {
    for(const auto& c : mClientData) {
        const int clientId = c.first;
        const auto u = unit(clientId);
        if(!u) continue;
        if(u->fTeamId == killed.fTeamId) continue;
        const float dist = ePointF::distance(u->fPos, killed.fPos);
        if(dist > 10.f) continue;
        u->killed(killed);
    }
}

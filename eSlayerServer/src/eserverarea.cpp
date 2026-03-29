#include "eserverarea.h"

#include "eclientaction.h"
#include "../../eSlayerHelpers/include/eSlayerHelpers/echardatainfo.h"
#include "eunitbaseaction.h"

#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/evectorhelpers.h>

#include <eSlayerMissiles/emissilecollision.h>
#include <eSlayerMissiles/emissileincrement.h>

eServerArea::eServerArea() :
    mUnitAreas(mUnitAreaDim),
    mItemAreas(mItemAreaDim) {}

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
            const auto u = std::make_shared<eServerUnit>(data);
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
    std::map<int, eUnitArea> clientAreas;

    for(const int i : mClientIds) {
        const auto u = unit(i);
        if(!u) continue;
        auto& oldArea = mClientAreas[i];
        const auto newArea = unitArea(*u);
        if(oldArea != newArea) {
            mUnitAreas.erase(oldArea, i);
            mUnitAreas.emplace(newArea, i);
            oldArea = newArea;
        }
        clientAreas[i] = newArea;
    }

    std::set<eUnitArea> unitTiles;
    for(const auto& clientAreaP : clientAreas) {
        const auto& clientArea = clientAreaP.second;
        for(int x = -mUnitAreaMargin; x <= mUnitAreaMargin; x++) {
            for(int y = -mUnitAreaMargin; y <= mUnitAreaMargin; y++) {
                const eUnitArea area{clientArea.fX + x, clientArea.fY + y};
                unitTiles.emplace(area);
            }
        }
    }
    for(const auto& area : unitTiles) {
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
        const auto oldPos = m->fPos;
        eMissileIncrement::increment(*m, by);
        const auto newPos = m->fPos;
        if(m->fRemDistTime <= 0.0001f) {
            removeMissile(m);
            continue;
        }
        const auto ipos = newPos.floor();
        const bool obsticle = !mMap->walkable(ipos.fX, ipos.fY);
        if(obsticle) {
            mMissiles.remove(m->fId);
            continue;
        }

        // Compute AABB of the travel segment, expanded by max
        // possible collision radius to cover all candidate units
        const float maxRadius = m->fRadius + 1.f;
        const float aabbMinX = std::min(oldPos.fX, newPos.fX) - maxRadius;
        const float aabbMaxX = std::max(oldPos.fX, newPos.fX) + maxRadius;
        const float aabbMinY = std::min(oldPos.fY, newPos.fY) - maxRadius;
        const float aabbMaxY = std::max(oldPos.fY, newPos.fY) + maxRadius;

        // Determine which unit areas overlap this AABB
        const auto areaMin = mUnitAreas.posArea(ePointF{aabbMinX, aabbMinY});
        const auto areaMax = mUnitAreas.posArea(ePointF{aabbMaxX, aabbMaxY});

        eMissileCollision::eResult collResult;

        for(int ax = areaMin.fX; ax <= areaMax.fX; ax++) {
            for(int ay = areaMin.fY; ay <= areaMax.fY; ay++) {
                const eUnitArea area{ax, ay};
                const auto& units = mUnitAreas.at(area);
                for(const int charId : units) {
                    const auto u = mUnits.get(charId);
                    if(!u) continue;
                    eMissileCollision::test(oldPos, newPos,
                                            *u, *m, collResult);
                    if(m->fContinuousDamage && collResult.fHit) {
                        const auto hitUnit = mUnits.get(collResult.fCharId);
                        if(hitUnit && m->fHitAction) m->fHitAction(*hitUnit);
                    }
                }
            }
        }

        if(!m->fContinuousDamage && collResult.fHit) {
            const auto hitUnit = mUnits.get(collResult.fCharId);
            if(hitUnit && m->fHitAction) m->fHitAction(*hitUnit);
            m->fPierced.emplace(collResult.fCharId);
            if(m->fToPierce == 0) continue;
            if(--m->fToPierce == 0) {
                mMissiles.remove(m->fId);
            }
        }
    }

    mTime += by;
}

std::vector<eUnitData>
eServerArea::unitsData(const int clientId) {
    std::vector<eUnitData> result;
    const auto client = unit(clientId);
    if(!client) return result;
    const auto clientArea = unitArea(*client);
    for(int x = -mUnitAreaMargin; x <= mUnitAreaMargin; x++) {
        for(int y = -mUnitAreaMargin; y <= mUnitAreaMargin; y++) {
            const eUnitArea area{clientArea.fX + x, clientArea.fY + y};
            const auto& units = mUnitAreas.at(area);
            for(const int charId : units) {
                const auto u = unit(charId);
                if(!u) continue;
                result.emplace_back(reinterpret_cast<eUnitData&>(*u));
            }
        }
    }
    return result;
}

std::vector<eGroundItem>
eServerArea::itemsData(const int clientId) {
    std::vector<eGroundItem> result;
    const auto client = unit(clientId);
    if(!client) return result;
    const auto clientArea = mItemAreas.posArea(client->fPos);
    for(int x = -mItemAreaMargin; x <= mItemAreaMargin; x++) {
        for(int y = -mItemAreaMargin; y <= mItemAreaMargin; y++) {
            const eUnitArea area{clientArea.fX + x, clientArea.fY + y};
            const auto& items = mItemAreas.at(area);
            for(const int itemId : items) {
                const auto i = groundItem(itemId);
                if(!i) continue;
                result.emplace_back(*i);
            }
        }
    }
    return result;
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

bool eServerArea::addClient(const int clientId,
                            const eEquipment& eq,
                            const ePointF& pos) {
    mClientIds.emplace(clientId);
    mClientLatestMissileId[clientId] = -1;
    const int typeId = 1;
    const auto& data = eCharDataInfo::get(typeId);
    const std::map<std::string, std::string> partsMap{{"whole", "light"}};
    const auto modelParts = data.mapToModelParts(partsMap);
    const auto u = std::make_shared<eServerUnit>(data);
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
    u->setEquipment(eq);
    mUnits.add(clientId, u);
    const auto area = unitArea(*u);
    mUnitAreas.emplace(area, clientId);
    mClientAreas[clientId] = area;
    return true;
}

bool eServerArea::removeClient(const int clientId) {
    removeUnit(clientId);
    mClientLatestMissileId.erase(clientId);
    const int r = mClientIds.erase(clientId);
    mClientAreas.erase(clientId);
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
    auto& eq = u->equipment();
    if(drag) {
        if(eq.fDragged.fType != eItemType::none) return false;
        eq.fDragged = *item;
    } else {
        const bool r = eq.add(*item);
        if(!r) return false;
    }
    mGroundItems.remove(itemId);
    mItemsOnGround.remove(itemId);
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
    const auto baseArea = mItemAreas.posArea(pos);
    bool found = false;
    for(int dist = 0; dist < 100; dist++) {
        for(int x = -dist; x <= dist; x++) {
            for(int y = -dist; y <= dist; y++) {
                if(std::abs(x) != dist && std::abs(y) != dist) continue;
                const eUnitArea area{baseArea.fX + x, baseArea.fY + y};
                const auto items = mItemAreas.at(area);
                if(!items.empty()) continue;
                mItemAreas.emplace(area, itemId);
                groundItem->fPos = ePointF{float(area.fX)/(-mItemAreaDim),
                                           float(area.fY)/(-mItemAreaDim)};
                found = true;
                break;
            }
            if(found) break;
        }
        if(found) break;
    }
    mGroundItems.add(itemId, groundItem);
    mItemsOnGround.add(itemId, std::make_shared<eItem>(item));
    item = eItem();
    return true;
}

void eServerArea::rearrangeItems(
    const int clientId, const eEquipment& eq) {
    const auto u = unit(clientId);
    if(!u) return;
    u->setEquipment(eq);
}

std::vector<eMissile>
eServerArea::missileData(const int clientId) const {
    std::vector<eMissile> result;
    const auto u = unit(clientId);
    if(!u) return result;
    result.reserve(mMissiles.actualSize());
    const auto latestMissile = mClientLatestMissileId[clientId];
    auto newLatestMissile = latestMissile;
    for(const auto& m : mMissiles) {
        if(m->fId <= latestMissile) continue;
        newLatestMissile = std::max(newLatestMissile, m->fId);
        const float dist = ePointF::distance(m->fPos, u->fPos);
        if(dist > 20.f) continue;
        result.emplace_back(*m);
    }
    mClientLatestMissileId[clientId] = newLatestMissile;
    return result;
}

void eServerArea::addMissile(const std::shared_ptr<eServerMissile>& m) {
    mMissiles.add(m->fId, m);
}

void eServerArea::removeMissile(const std::shared_ptr<eServerMissile>& m) {
    mMissiles.remove(m->fId);
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

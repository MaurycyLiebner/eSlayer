#include "eserverarea.h"

#include "eclientaction.h"
#include "eserverchardata.h"
#include "eunitbaseaction.h"

#include <eSlayerHelpers/erand.h>
#include <eSlayerHelpers/evectorhelpers.h>
#include <eSlayerMissiles/emissileincrement.h>

void eServerArea::initialize(const std::shared_ptr<eMap>& map) {
    mMap = map;

    for(int x = 24; x < 75; x++) {
        if(x == 40) x += 20;
        for(int y = 24; y < 75; y++) {
            if(y == 40) y += 20;
            const int typeId = 2 + eRand::rand() % 2;
            const auto data = eServerCharData::get(typeId);
            const auto name = data->name();
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
            const auto modelParts = data->mapToModelParts(partsMap);
            const auto u = std::make_shared<eServerUnit>(*data);
            const int charId = eServerUnit::sNextCharId++;
            u->fCharId = charId;
            u->fTeamId = -1;
            u->fTypeId = typeId;
            u->fModelParts = modelParts;
            u->fHealth = 100;
            u->fMaxHealth = 100;
            u->fRadius = data->radius();
            u->fAnim = data->animId("stand");
            u->fActionTime = 0.f;
            u->fAnimId = 0;
            const ePointF pos{float(x), float(y)};
            u->fPos = pos;
            u->fAngle = 0.f;
            mUnits.add(charId, u);
            const auto area = unitArea(*u);
            mUnitAreas[area].emplace(charId);

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
    std::map<int, eUnitTile> clientAreas;

    for(const int i : mClientIds) {
        const auto u = unit(i);
        if(!u) continue;
        auto& oldArea = mClientAreas[i];
        const auto newArea = unitArea(*u);
        if(oldArea != newArea) {
            mUnitAreas[oldArea].erase(i);
            mUnitAreas[newArea].emplace(i);
            oldArea = newArea;
        }
        clientAreas[i] = newArea;
    }

    std::set<eUnitTile> unitTiles;
    for(const auto& clientAreaP : clientAreas) {
        const auto& clientArea = clientAreaP.second;
        for(int x = -mUnitAreaMargin; x <= mUnitAreaMargin; x++) {
            for(int y = -mUnitAreaMargin; y <= mUnitAreaMargin; y++) {
                const eUnitTile area{clientArea.fX + x, clientArea.fY + y};
                const auto it = mUnitAreas.find(area);
                if(it == mUnitAreas.end()) continue;
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
                mUnitAreas[oldArea].erase(charId);
                mUnitAreas[newArea].emplace(charId);
            }
        }
    }

    for(const auto& m : mMissiles) {
        const auto oldPos = m->fPos;
        eMissileIncrement::increment(*m, by);
        const auto newPos = m->fPos;
        if(m->fRemDist <= 0.0001f) {
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
        const auto areaMin = posArea(ePointF{aabbMinX, aabbMinY});
        const auto areaMax = posArea(ePointF{aabbMaxX, aabbMaxY});

        // Segment direction vector
        const float dx = newPos.fX - oldPos.fX;
        const float dy = newPos.fY - oldPos.fY;
        const float segLenSq = dx*dx + dy*dy;

        float bestT = 2.f; // > 1 means no hit found
        int bestCharId = -1;

        for(int ax = areaMin.fX; ax <= areaMax.fX; ax++) {
            for(int ay = areaMin.fY; ay <= areaMax.fY; ay++) {
                const auto it = mUnitAreas.find(eUnitTile{ax, ay});
                if(it == mUnitAreas.end()) continue;
                for(const int charId : it->second) {
                    const auto u = mUnits.get(charId);
                    if(!u || u->fHealth <= 0) continue;
                    if(u->fTeamId == m->fTeamId) continue;
                    const float collR = 0.5f*(u->fRadius + m->fRadius);

                    if(segLenSq < 0.0001f) {
                        // Missile barely moved — point test
                        const float dist = ePointF::distance(u->fPos, newPos);
                        if(dist <= collR && 0.f < bestT) {
                            bestT = 0.f;
                            bestCharId = charId;
                        }
                    } else {
                        // Swept segment-vs-circle test
                        // Solve: |oldPos + t*d - C|² = collR²
                        const float fx = oldPos.fX - u->fPos.fX;
                        const float fy = oldPos.fY - u->fPos.fY;
                        const float a = segLenSq;
                        const float b = 2.f*(fx*dx + fy*dy);
                        const float c = fx*fx + fy*fy - collR*collR;
                        const float disc = b*b - 4.f*a*c;
                        if(disc < 0.f) continue;
                        float t = (-b - std::sqrt(disc)) / (2.f*a);
                        // If entry point is behind us, check if
                        // we're already inside the circle at t=0
                        if(t < 0.f) t = 0.f;
                        if(t <= 1.f && t < bestT) {
                            bestT = t;
                            bestCharId = charId;
                        }
                    }
                }
            }
        }

        if(bestCharId >= 0) {
            // Move missile to the hit point
            m->fPos.fX = oldPos.fX + bestT * dx;
            m->fPos.fY = oldPos.fY + bestT * dy;
            const auto hitUnit = mUnits.get(bestCharId);
            if(hitUnit && m->fHitAction) m->fHitAction(*hitUnit);
            mMissiles.remove(m->fId);
        }
    }

    mTime += by;
}

std::vector<eUnitData>
eServerArea::unitsData(const int clientId) const {
    std::vector<eUnitData> result;
    const auto client = unit(clientId);
    if(!client) return result;
    const auto clientArea = unitArea(*client);
    for(int x = -mUnitAreaMargin; x <= mUnitAreaMargin; x++) {
        for(int y = -mUnitAreaMargin; y <= mUnitAreaMargin; y++) {
            const eUnitTile area{clientArea.fX + x, clientArea.fY + y};
            const auto it = mUnitAreas.find(area);
            if(it == mUnitAreas.end()) continue;
            for(const int charId : it->second) {
                const auto u = unit(charId);
                if(!u) continue;
                result.emplace_back(reinterpret_cast<eUnitData&>(*u));
            }
        }
    }
    return result;
}

eUnitTile eServerArea::unitArea(const int charId) const {
    const auto u = unit(charId);
    if(!u) return {0, 0};
    return unitArea(*u);
}

eUnitTile eServerArea::unitArea(const eServerUnit& u) const {
    const auto& pos = u.fPos;
    return posArea(pos);
}

eUnitTile eServerArea::posArea(const ePointF& pos) const {
    eUnitTile result;
    reinterpret_cast<ePoint&>(result) = pos.floor()/mUnitAreaDim;
    return result;
}

bool eServerArea::addClient(const int clientId, const ePointF& pos) {
    mClientIds.emplace(clientId);
    mClientLatestMissileId[clientId] = -1;
    const int typeId = 1;
    const auto data = eServerCharData::get(typeId);
    const std::map<std::string, std::string> partsMap{{"whole", "light"}};
    const auto modelParts = data->mapToModelParts(partsMap);
    const auto u = std::make_shared<eServerUnit>(*data);
    u->fCharId = clientId;
    u->fTypeId = typeId;
    u->fRadius = data->radius();
    u->fAnim = data->animId("stand");
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
    mUnits.add(clientId, u);
    const auto area = unitArea(*u);
    mUnitAreas[area].emplace(clientId);
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
    mUnitAreas[area].erase(charId);
    return mUnits.remove(charId);
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

std::shared_ptr<eServerUnit>
eServerArea::unit(const ePointF& pos) const {
    const auto area = posArea(pos);
    const auto it = mUnitAreas.find(area);
    if(it == mUnitAreas.end()) return nullptr;
    for(const auto& u : mUnitAreas) {
        for(const int charId : u.second) {
            const auto u = unit(charId);
            if(!u) continue;
            const auto& upos = u->fPos;
            const float dist = ePointF::distance(pos, upos);
            if(dist <= u->fRadius) return u;
        }
    }
    return nullptr;
}

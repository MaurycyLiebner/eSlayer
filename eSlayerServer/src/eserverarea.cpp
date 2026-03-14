#include "eserverarea.h"

#include "eunitbaseaction.h"
#include "eserverchardata.h"
#include "eclientaction.h"

#include <eSlayerHelpers/erand.h>

void eServerArea::initialize(const std::shared_ptr<eMap>& map) {
    mMap = map;

    for(int x = 24; x < 75; x++) {
        if(x == 40) x += 20;
        for(int y = 24; y < 75; y++) {
            if(y == 40) y += 20;
            const int typeId = eRand::rand() % 2;
            const std::string name = typeId == 0 ? "mummy" : "wendigo";
            const auto data = eServerCharData::get(name);
            const auto u = std::make_shared<eServerUnit>(*data);
            const int charId = eServerUnit::sNextCharId++;
            u->fCharId = charId;
            u->fTeamId = -1;
            u->fTypeId = typeId;
            u->fHealth = 100;
            u->fMaxHealth = 100;
            u->fRadius = data->radius();
            u->fAnim = data->animId("stand");
            u->fActionTime = 0.;
            u->fAnimId = 0;
            const ePointF pos{double(x), double(y)};
            u->fPos = pos;
            mUnitIdMap[charId] = mUnits.size();
            mUnits.emplace_back(u);
            const auto area = unitArea(*u);
            mUnitAreas[area].emplace(charId);

            auto& m = u->movementHandler();
            m.setSpeed(0.025);

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

void eServerArea::increment(const double by) {
    std::map<int, eUnitTile> clientAreas;

    for(const int i : mClientIds) {
        const auto u = unit(i);
        if(!u) continue;
        const auto area = unitArea(*u);
        clientAreas[i] = area;
    }

    std::set<eUnitTile> unitTiles;
    for(const auto& clientAreaP : clientAreas) {
        const auto& clientArea = clientAreaP.second;
        const int iMax = mUnits.size();
        for(int i = 0; i < iMax; i++) {
            for(int x = -mUnitAreaMargin; x <= mUnitAreaMargin; x++) {
                for(int y = -mUnitAreaMargin; y <= mUnitAreaMargin; y++) {
                    const eUnitTile area{clientArea.fX + x, clientArea.fY + y};
                    const auto it = mUnitAreas.find(area);
                    if(it == mUnitAreas.end()) continue;
                    unitTiles.emplace(area);
                }
            }
        }
    }
    for(const auto& area : unitTiles) {
        const auto units = mUnitAreas.at(area);
        for(const int charId : units) {
            const int id = mUnitIdMap[charId];
            const auto& u = mUnits[id];
            const auto oldArea = unitArea(*u);
            u->increment(by);
            const auto newArea = unitArea(*u);
            if(oldArea != newArea) {
                mUnitAreas[oldArea].erase(charId);
                mUnitAreas[newArea].emplace(charId);
            }
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
    if(!u) return {-1, -1};
    return unitArea(*u);
}

eUnitTile eServerArea::unitArea(const eServerUnit& u) const {
    const auto& pos = u.fPos;
    eUnitTile result;
    reinterpret_cast<ePoint&>(result) = pos.floor()/mUnitAreaDim;
    return result;
}

void eServerArea::addClient(
    const int clientId, const ePointF& pos) {
    mClientIds.emplace_back(clientId);
    mUnitIdMap[clientId] = mUnits.size();
    const std::string name = "pal";
    const auto data = eServerCharData::get(name);
    const auto u = std::make_shared<eServerUnit>(*data);
    u->fCharId = clientId;
    u->fRadius = data->radius();
    u->fAnim = data->animId("stand");
    u->fAnimId = 0;
    u->fTeamId = 0;
    u->fPos = pos;
    u->fMaxHealth = 100;
    u->fHealth = 100;
    u->fActionTime = 0.;
    const auto a = std::make_shared<eClientAction>(*u, *this);
    u->setAction(a);
    mUnits.emplace_back(u);
    const auto area = unitArea(*u);
    mUnitAreas[area].emplace(clientId);
}

std::shared_ptr<eServerUnit>
eServerArea::unit(const int charId) const {
    const auto it = mUnitIdMap.find(charId);
    if(it == mUnitIdMap.end()) return nullptr;
    const int id = it->second;
    return mUnits[id];
}

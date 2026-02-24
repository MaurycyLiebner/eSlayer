#include "eserverarea.h"

#include <eSlayerHelpers/erand.h>

void eServerArea::initialize(const std::shared_ptr<eMap>& map) {
    mMap = map;

    for(int x = 12; x < 24; x++) {
        for(int y = 12; y < 24; y++) {
            const auto u = std::make_shared<eServerUnit>();
            const int charId = eServerUnit::sNextCharId++;
            u->fCharId = charId;
            u->fTeamId = -1;
            const ePointF pos{double(x), double(y)};
            u->fPos = pos;
            mUnitIdMap[charId] = mUnits.size();
            mUnits.emplace_back(u);

            const auto m = std::make_shared<eMovementHandler>();
            m->intialize(charId);
            m->setPos(pos);
            m->setWalkable([this](const int x, const int y) {
                return mMap->walkable(x, y);
            });
            m->setObsticle([this](const int charId, const ePointF& p) {
                for(const auto& u : mUnits) {
                    if(charId == u->fCharId) continue;
                    const auto& pos = u->fPos;
                    const auto dist = ePointF::distance(pos, p);
                    if(dist < 0.25) return true;
                }
                return false;
            });
            mMovementHandlers.emplace_back(m);
        }
    }
}

void eServerArea::increment() {
    const int iMax = mMovementHandlers.size();
    for(int i = 0; i < iMax; i++) {
        const auto& m = mMovementHandlers[i];
        if(!m) continue;
        m->increment();
        const auto& u = mUnits[i];
        const auto newPos = m->pos();
        u->fDir = eVec2d{newPos.fX - u->fPos.fX,
                         newPos.fY - u->fPos.fY};
        u->fPos = newPos;

        if(eRand::rand() % 11 == 10) {
            for(const auto& uu : mUnits) {
                if(u->fTeamId == uu->fTeamId) continue;
                const double dist = ePointF::distance(u->fPos, uu->fPos);
                if(dist < 5.) {
                    m->moveTo(uu->fPos);
                    break;
                }
            }
        }
    }
}

void eServerArea::addClient(
    const int clientId, const ePointF& pos) {
    mUnitIdMap[clientId] = mUnits.size();
    const auto u = std::make_shared<eServerUnit>();
    u->fCharId = clientId;
    u->fTeamId = 0;
    u->fPos = pos;
    mUnits.emplace_back(u);
    mMovementHandlers.emplace_back();
}

std::shared_ptr<eServerUnit>
eServerArea::unit(const int charId) const {
    const auto it = mUnitIdMap.find(charId);
    if(it == mUnitIdMap.end()) return nullptr;
    const int id = it->second;
    return mUnits[id];
}

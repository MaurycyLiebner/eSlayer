#include "eserverarea.h"

#include <eSlayerHelpers/erand.h>

void eServerArea::initialize(const std::shared_ptr<eMap>& map) {
    mMap = map;

    for(int x = 13; x < 24; x++) {
        for(int y = 13; y < 24; y++) {
            const auto u = std::make_shared<eServerUnit>();
            const int charId = eServerUnit::sNextCharId++;
            u->fCharId = charId;
            u->fTeamId = -1;
            u->fRadius = 0.4;
            const ePointF pos{double(x), double(y)};
            u->fPos = pos;
            mUnitIdMap[charId] = mUnits.size();
            mUnits.emplace_back(u);

            const auto m = std::make_shared<eMovementHandler>();
            m->setSpeed(0.025);

            const auto w = [this](const int x, const int y) {
                return mMap->walkable(x, y);
            };
            const auto iter = [this, charId](const eOtherHandler& handler) {
                for(const auto& u : mUnits) {
                    if(charId == u->fCharId) continue;
                    handler(*u);
                }
            };
            m->intialize(w, iter, charId);
            m->setRadius(u->fRadius);
            m->setPos(pos);
            mMovementHandlers.emplace_back(m);
        }
    }
}

void eServerArea::increment() {
    const int iMax = mMovementHandlers.size();
    for(int i = 0; i < iMax; i++) {
        const auto& m = mMovementHandlers[i];
        if(!m) continue;
        const bool r = m->increment(1.);
        const auto& u = mUnits[i];
        if(r) {
            const auto newPos = m->pos();
            u->fVel = ePointF::vector(newPos, u->fPos);
            u->fPos = newPos;
        } else {
            m->stopMoving();
            u->fVel = eVec2d{0., 0.};
        }

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
    u->fRadius = 0.4;
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

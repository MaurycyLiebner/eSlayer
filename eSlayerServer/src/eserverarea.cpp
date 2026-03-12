#include "eserverarea.h"

#include "eunitbaseaction.h"
#include "eserverchardata.h"

#include <eSlayerHelpers/erand.h>

void eServerArea::initialize(const std::shared_ptr<eMap>& map) {
    mMap = map;

    for(int x = 13; x < 24; x++) {
        for(int y = 13; y < 24; y++) {
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
    const int iMax = mUnits.size();
    for(int i = 0; i < iMax; i++) {
        const auto& u = mUnits[i];
        u->increment(by);
    }
    mTime += by;
}

void eServerArea::addClient(
    const int clientId, const ePointF& pos) {
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
    mUnits.emplace_back(u);
}

std::shared_ptr<eServerUnit>
eServerArea::unit(const int charId) const {
    const auto it = mUnitIdMap.find(charId);
    if(it == mUnitIdMap.end()) return nullptr;
    const int id = it->second;
    return mUnits[id];
}

#include "eserverarea.h"

#include <eSlayerMapGenerator/emap.h>

void eServerArea::initialize(const std::shared_ptr<eMap>& map) {
    mMap = map;

    const int w = map->width();
    const int h = map->height();
    mUnitAreas.initialize(w, h, mUnitAreaDim);
    mItemAreas.initialize(w, h, mItemAreaDim);
    mItemTiles.initialize(w, h, -mItemTileSubdivision);

    addUnits();
}

void eServerArea::iniMissileInc() {
    const auto obstacle = [this](const ePointF& pos) {
        return mMap->obstacle(pos);
    };

    const auto removeMissile = [this](const eMissile& m) {
        const auto& mptr = mMissiles.get(m.fId);
        for(auto& it : mClientData) {
            auto& data = it.second;
            data.fRemovedMissiles.add(m.fId, mptr);
        }
        mMissiles.remove(m.fId);
    };

    const auto getUnit = [this](const uint32_t charId) {
        const auto u = mUnits.get(charId);
        return static_cast<eUnitData*>(u.get());
    };

    const auto getMissile = [this](const uint32_t mid) {
        const auto m = mMissiles.get(mid);
        return static_cast<eMissile*>(m.get());
    };

    const auto hitAction = [this](const eMissile& m, eUnitData& u) {
        const auto& sm = static_cast<const eServerMissile&>(m);
        auto& su = static_cast<eServerUnit&>(u);
        if(sm.fHitAction) sm.fHitAction(su, sm);
    };

    mMIncrementer.initialize(obstacle,
                             removeMissile,
                             getUnit,
                             getMissile,
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

    const auto getUnit = [this](const uint32_t charId) {
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
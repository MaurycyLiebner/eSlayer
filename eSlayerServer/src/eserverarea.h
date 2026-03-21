#ifndef ESERVERAREA_H
#define ESERVERAREA_H

#include "eserverunit.h"
#include "eservermissile.h"

#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/eidmapvector.h>
#include <eSlayerHelpers/eunitarea.h>
#include <eSlayerHelpers/eunitareas.h>

#include <memory>
#include <map>
#include <set>

class eServerArea {
public:
    eServerArea();

    void initialize(const std::shared_ptr<eMap>& map);

    void increment(const float by = 1.f);

    float time() const { return mTime; }

    const eIdMapVector<eServerUnit>&
    units() const { return mUnits; }

    std::vector<eUnitData>
    unitsData(const int clientId);

    eUnitArea unitArea(const int charId) const;
    eUnitArea unitArea(const eServerUnit& u) const;

    bool addClient(const int clientId, const ePointF& pos);
    bool removeClient(const int clientId);
    bool removeUnit(const int charId);

    std::vector<eMissile>
    missileData(const int clientId) const;

    void addMissile(const std::shared_ptr<eServerMissile>& m);
    void removeMissile(const std::shared_ptr<eServerMissile>& m);

    std::shared_ptr<eServerUnit>
    unit(const int charId) const;
    std::shared_ptr<eServerUnit>
    unit(const ePointF& pos);
private:
    float mTime = 0.f;

    eIdMapVector<eServerMissile> mMissiles;
    mutable std::map<int, int32_t> mClientLatestMissileId;
    eIdMapVector<eServerUnit> mUnits;
    const int mUnitAreaDim = 4;
    const int mUnitAreaMargin = 3;
    eUnitAreas mUnitAreas;
    std::map<int, eUnitArea> mClientAreas;
    std::set<int> mClientIds;

    std::shared_ptr<eMap> mMap;
};

#endif // ESERVERAREA_H

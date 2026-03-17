#ifndef ESERVERAREA_H
#define ESERVERAREA_H

#include "eserverunit.h"

#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/eidmapvector.h>
#include <eSlayerHelpers/emissile.h>

#include <memory>
#include <map>
#include <set>

struct eUnitTile : public ePoint {
   friend bool operator<(const eUnitTile& t1, const eUnitTile& t2) {
        if(t1.fY != t2.fY) return t1.fY < t2.fY;
        return t1.fX < t2.fX;
    }
};

class eServerArea {
public:
    void initialize(const std::shared_ptr<eMap>& map);

    void increment(const float by = 1.f);

    float time() const { return mTime; }

    const eIdMapVector<eServerUnit>&
    units() const { return mUnits; }

    std::vector<eUnitData>
    unitsData(const int clientId) const;

    eUnitTile unitArea(const int charId) const;
    eUnitTile unitArea(const eServerUnit& u) const;
    eUnitTile posArea(const ePointF& pos) const;

    bool addClient(const int clientId, const ePointF& pos);
    bool removeClient(const int clientId);
    bool removeUnit(const int charId);

    std::vector<eMissile>
    missileData(const int clientId) const;

    void addMissile(const std::shared_ptr<eMissile>& m);
    void removeMissile(const std::shared_ptr<eMissile>& m);

    std::shared_ptr<eServerUnit>
    unit(const int charId) const;
    std::shared_ptr<eServerUnit>
    unit(const ePointF& pos) const;
private:
    float mTime = 0.f;

    std::vector<std::shared_ptr<eMissile>> mMissiles;
    eIdMapVector<eServerUnit> mUnits;
    std::map<eUnitTile, std::set<int>> mUnitAreas;
    std::map<int, eUnitTile> mClientAreas;
    std::set<int> mClientIds;
    const int mUnitAreaDim = 4;
    const int mUnitAreaMargin = 3;

    std::shared_ptr<eMap> mMap;
};

#endif // ESERVERAREA_H

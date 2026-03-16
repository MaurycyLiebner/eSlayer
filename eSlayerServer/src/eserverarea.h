#ifndef ESERVERAREA_H
#define ESERVERAREA_H

#include "eserverunit.h"

#include <eSlayerMapGenerator/emapgenerator.h>
#include <eSlayerHelpers/emovementhandler.h>

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

    const std::vector<std::shared_ptr<eServerUnit>>&
    units() const { return mUnits; }

    std::vector<eUnitData>
    unitsData(const int clientId) const;

    eUnitTile unitArea(const int charId) const;
    eUnitTile unitArea(const eServerUnit& u) const;
    eUnitTile posArea(const ePointF& pos) const;

    void addClient(const int clientId, const ePointF& pos);

    std::shared_ptr<eServerUnit>
    unit(const int charId) const;
    std::shared_ptr<eServerUnit>
    unit(const ePointF& pos) const;
private:
    float mTime = 0.f;

    std::vector<std::shared_ptr<eServerUnit>>
    mUnits;
    std::map<int, int> mUnitIdMap;
    std::map<eUnitTile, std::set<int>> mUnitAreas;
    std::map<int, eUnitTile> mClientAreas;
    std::vector<int> mClientIds;
    const int mUnitAreaDim = 4;
    const int mUnitAreaMargin = 3;

    std::shared_ptr<eMap> mMap;
};

#endif // ESERVERAREA_H

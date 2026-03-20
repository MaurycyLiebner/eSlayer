#ifndef EGAMEWORLD_H
#define EGAMEWORLD_H

#include "units/eunit.h"

#include <eSlayerHelpers/eunitdata.h>

#include <eSlayerHelpers/eidmapvector.h>
#include <eSlayerHelpers/emissile.h>
#include <eSlayerHelpers/eunittile.h>
#include <eSlayerServer/eserver.h>

#include <map>
#include <set>

class eMap;

class eGameWorld {
public:
    struct eProcessResult {
        bool fReceived = false;
        bool fAggressive = false;
        bool fHasMainCharData = false;
        eUnitData fMainCharData;
    };

    eProcessResult processServerData(
        const int clientId,
        const std::shared_ptr<eServer>& server,
        const std::shared_ptr<eUnit>& mainChar,
        SDL_Renderer* const r);

    void simulateMissiles(
        const float by,
        const std::shared_ptr<eMap>& map);

    eIdMapVector<eUnit>& units() { return mUnits; }
    const eIdMapVector<eUnit>& units() const { return mUnits; }

    eIdMapVector<eExtendedMissile>& missiles() { return mMissiles; }
    const eIdMapVector<eExtendedMissile>& missiles() const { return mMissiles; }

    const std::map<eUnitTile, std::set<int>>& unitAreas() const {
        return mUnitAreas;
    }

    std::shared_ptr<eUnit> getUnit(const int id) const {
        return mUnits.get(id);
    }
private:
    eIdMapVector<eUnit> mUnits;
    eIdMapVector<eExtendedMissile> mMissiles;
    std::map<eUnitTile, std::set<int>> mUnitAreas;
};

#endif // EGAMEWORLD_H

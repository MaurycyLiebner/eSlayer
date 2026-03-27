#ifndef EGAMEWORLD_H
#define EGAMEWORLD_H

#include "units/eunit.h"

#include <eSlayerHelpers/eunitdata.h>

#include <eSlayerHelpers/eidmapvector.h>
#include <eSlayerHelpers/emissile.h>
#include <eSlayerHelpers/eunitareas.h>
#include <eSlayerServer/eserver.h>

class eMap;
class eMainCharAction;

struct eExtendedMissile : public eMissile {
    float fAngle;
};

class eGameWorld {
public:
    eGameWorld();

    struct eProcessResult {
        bool fReceived = false;
        bool fAggressive = false;
        bool fHasMainCharData = false;
        eUnitData fMainCharData;
    };

    eProcessResult processServerData(
        const int clientId,
        eServer& server,
        const eUnit& mainChar,
        eMainCharAction& mainAct,
        SDL_Renderer* const r);

    void simulateMissiles(
        const float by,
        const std::shared_ptr<eMap>& map);

    eIdMapVector<eUnit>& units() { return mUnits; }
    const eIdMapVector<eUnit>& units() const { return mUnits; }

    eIdMapVector<eExtendedMissile>& missiles() { return mMissiles; }
    const eIdMapVector<eExtendedMissile>& missiles() const { return mMissiles; }

    std::shared_ptr<eUnit> getUnit(const int id) const {
        return mUnits.get(id);
    }
private:
    eIdMapVector<eUnit> mUnits;
    eIdMapVector<eExtendedMissile> mMissiles;
    eUnitAreas mUnitAreas;
};

#endif // EGAMEWORLD_H

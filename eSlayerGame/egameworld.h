#ifndef EGAMEWORLD_H
#define EGAMEWORLD_H

#include "units/eunit.h"

#include <eSlayerMissiles/emissileincrementer.h>

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eidmapvector.h>
#include <eSlayerHelpers/emissile.h>
#include <eSlayerHelpers/eunitareas.h>
#include <eSlayerServer/eserver.h>
#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/egrounditem.h>

class eMap;
class eMainCharAction;

struct eExtendedMissile : public eMissile {
    float fAngle;
};

class eGameWorld {
public:
    eGameWorld(const std::shared_ptr<eMap>& map);

    struct eProcessResult {
        bool fReceived = false;
        bool fAggressive = false;
        bool fHasMainCharData = false;
        eUnitData fMainCharData;
        uint16_t fMana = 0;
        uint16_t fLevel = 0;
        uint16_t fExperience = 0;
    };

    eProcessResult processServerData(
        const int clientId,
        eServer& server,
        const eUnit& mainChar,
        eMainCharAction& mainAct,
        SDL_Renderer* const r);

    void simulateMissiles(const float by);

    eIdMapVector<eUnit>& units() { return mUnits; }
    const eIdMapVector<eUnit>& units() const { return mUnits; }

    eIdMapVector<eExtendedMissile>& missiles() { return mMissiles; }
    const eIdMapVector<eExtendedMissile>& missiles() const { return mMissiles; }

    std::shared_ptr<eUnit> getUnit(const int id) const {
        return mUnits.get(id);
    }

    eIdMapVector<eGroundItem>& groundItems() { return mGroundItems; }
    const eIdMapVector<eGroundItem>& groundItems() const { return mGroundItems; }
private:
    const std::shared_ptr<eMap>& mMap;
    eIdMapVector<eUnit> mUnits;
    eIdMapVector<eExtendedMissile> mMissiles;
    eIdMapVector<eGroundItem> mGroundItems;
    eUnitAreas mUnitAreas;
    eMissileIncrementer mMIncrementer;
};

#endif // EGAMEWORLD_H

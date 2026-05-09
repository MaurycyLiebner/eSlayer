#ifndef EGAMEWORLD_H
#define EGAMEWORLD_H

#include "units/eunit.h"

#include <eSlayerMissiles/emissileincrementer.h>
#include <eSlayerMissiles/enovaincrementer.h>

#include <eSlayerHelpers/eunitdata.h>
#include <eSlayerHelpers/eidmapvector.h>
#include <eSlayerHelpers/emissile.h>
#include <eSlayerHelpers/efixedsizesetareas.h>
#include <eSlayerServer/eserver.h>
#include <eSlayerHelpers/eitem.h>
#include <eSlayerHelpers/egrounditem.h>
#include <eSlayerHelpers/enova.h>

class eMap;
class eMainCharAction;

struct eExtendedMissile : public eMissile {
    float fAngle;
};

class eGameWorld {
public:
    eGameWorld(const std::shared_ptr<eMap>& map);

    void initialize(const int clientId,
                    const std::shared_ptr<eUnit>& mainChar);

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
        const eResolution& res,
        SDL_Renderer* const r);

    void simulateMissiles(const float by);
    void simulateNovas(const float by);

    eIdMapVector<eUnit>& units() { return mUnits; }
    const eIdMapVector<eUnit>& units() const { return mUnits; }

    eIdMapVector<eExtendedMissile>& missiles() { return mMissiles; }
    const eIdMapVector<eExtendedMissile>& missiles() const { return mMissiles; }

    eIdMapVector<eNova>& novas() { return mNovas; }
    const eIdMapVector<eNova>& novas() const { return mNovas; }

    std::shared_ptr<eUnit> getUnit(const int id) const {
        return mUnits.get(id);
    }

    std::shared_ptr<eGroundItem> getItem(const int id) const {
        return mGroundItems.get(id);
    }

    eIdMapVector<eGroundItem>& groundItems() { return mGroundItems; }
    const eIdMapVector<eGroundItem>& groundItems() const { return mGroundItems; }

    bool isBody(const int charId) const;
private:
    void iniMissileInc();
    void iniNovaInc();

    const std::shared_ptr<eMap>& mMap;
    int mClientId;
    std::shared_ptr<eUnit> mMainChar;
    eIdMapVector<eUnit> mUnits;
    eIdMapVector<eExtendedMissile> mMissiles;
    eIdMapVector<eNova> mNovas;
    eIdMapVector<eGroundItem> mGroundItems;
    eFixedSizeSetAreas mUnitAreas;
    eMissileIncrementer mMIncrementer;
    eNovaIncrementer mNIncrementer;
    std::vector<int> mBodies;
};

#endif // EGAMEWORLD_H

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
#include <eSlayerHelpers/eskillarea.h>

struct eBody;
class eMap;
class eMainCharAction;

struct eExtendedMissile : public eMissile {
    float fAngle;
    int fAnimId;
    int fFrame = 0;
    bool fHit = false;
};

struct eExtendedNova : public eNova {
    bool fInitialized = false;
    int fFrame = 0;
    std::vector<std::shared_ptr<eExtendedMissile>> fMissiles;
};

struct eExtendedSkillArea : public eSkillArea {
    int fFrame = 0;
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
        bool fUpdateBoostsAuras = false;
        std::multimap<eBoostCurseType, eModifier> fBoosts;
        std::multimap<eAuraType, eModifier> fAuras;
    };

    eProcessResult processServerData(
        const int clientId,
        eServer& server,
        const eUnit& mainChar,
        eMainCharAction& mainAct,
        const eResolution& res,
        SDL_Renderer* const r,
        const std::vector<eBody>& bodies);

    void simulateMissiles(const float by);
    void simulateNovas(const float by);
    void simulateSkillAreas(const float by);

    eIdMapVector<eUnit>& units() { return mUnits; }
    const eIdMapVector<eUnit>& units() const { return mUnits; }

    eIdMapVector<eExtendedMissile>& missiles() { return mMissiles; }
    const eIdMapVector<eExtendedMissile>& missiles() const { return mMissiles; }

    eIdMapVector<eExtendedNova>& novas() { return mNovas; }
    const eIdMapVector<eExtendedNova>& novas() const { return mNovas; }

    eIdMapVector<eExtendedSkillArea>& skillAreas() { return mSkillAreas; }
    const eIdMapVector<eExtendedSkillArea>& skillAreas() const { return mSkillAreas; }

    std::shared_ptr<eUnit> getUnit(const int id) const {
        return mUnits.get(id);
    }

    std::shared_ptr<eGroundItem> getItem(const int id) const {
        return mGroundItems.get(id);
    }

    eIdMapVector<eGroundItem>& groundItems() { return mGroundItems; }
    const eIdMapVector<eGroundItem>& groundItems() const { return mGroundItems; }

    void removeMissile(const eMissile& m);;
private:
    void iniMissileInc();
    void iniNovaInc();

    void addUnit(const ePointF& pos, const int charId);

    const std::shared_ptr<eMap>& mMap;
    int mClientId;
    std::shared_ptr<eUnit> mMainChar;
    eIdMapVector<eUnit> mUnits;
    eIdMapVector<eExtendedMissile> mMissiles;
    eIdMapVector<eExtendedNova> mNovas;
    eIdMapVector<eExtendedSkillArea> mSkillAreas;
    eIdMapVector<eGroundItem> mGroundItems;
    eFixedSizeSetAreas mUnitAreas;
    std::set<eArea> mUsedUnitAreas;
    eMissileIncrementer mMIncrementer;
    eNovaIncrementer mNIncrementer;

    eProcessResult mResult;
};

#endif // EGAMEWORLD_H

#ifndef ESERVERAREA_H
#define ESERVERAREA_H

#include "eserverunit.h"
#include "eservermissile.h"

#include <eSlayerMissiles/emissileincrementer.h>

#include <eSlayerHelpers/eunitdynamicdata.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/eidmapvector.h>
#include <eSlayerHelpers/earea.h>
#include <eSlayerHelpers/eareas.h>
#include <eSlayerHelpers/esetareas.h>
#include <eSlayerHelpers/escreendimensions.h>

#include <eSlayerMapGenerator/emapgenerator.h>

#include <memory>
#include <map>
#include <set>

class eCharacter;
struct eUnitInfo;

struct eClientData {
    eClientData();
    // in tile dimensions
    eScreenDimensions fScreen;
    eArea fArea;
    std::set<int> fKnownUnits;
    std::set<int> fKnownItems;
    int32_t fLatestMissile;
    eAreas fKnownMap;
    int fKnownBodies = 0;
    std::vector<int> fBodies;
};

class eServerArea {
public:
    eServerArea();

    void initialize(const std::shared_ptr<eMap>& map);

    void increment(const float by = 1.f);

    float time() const { return mTime; }

    const eIdMapVector<eServerUnit>&
    units() const { return mUnits; }

    void unitsData(const int clientId,
                   std::vector<eUnitData>& newUnits,
                   std::vector<eUnitDynamicData>& updatedUnits);
    void itemsData(const int clientId,
                   std::vector<eGroundItem>& newItems,
                   std::vector<uint32_t>& removedItemIds);
    std::vector<eMissile>
    missileData(const int clientId);
    std::vector<int>
    bodies(const int clientId);

    eArea unitArea(const int charId) const;
    eArea unitArea(const eServerUnit& u) const;

    eArea itemArea(const int itemId) const;
    eArea itemArea(const eGroundItem& i) const;

    eArea itemTile(const int itemId) const;
    eArea itemTile(const eGroundItem& i) const;

    bool mapPortions(const int clientId,
                     std::vector<eMapPortion>& result);

    bool addClient(const int clientId,
                   eCharacter& c,
                   const ePointF& pos,
                   const eScreenDimensions& screenDims);
    bool respawn(const int clientId);
    bool removeClient(const int clientId);
    bool planRemoveUnit(const int charId);
    bool pickupBody(const int clientId, const int charId);

    bool pickupItem(const int clientId, const int itemId,
                    const bool drag);
    bool dropItem(const int clientId, const int itemId);
    void rearrangeItems(const int clientId, const eEquipment& eq);
    void changeAttributes(const int clientId, const eAttributes& attrs);
    void changeSkillLevels(const int clientId, const eSkillLevels& skillLevels);
    void consumePotion(const int clientId, const uint32_t itemId);

    void addMissile(const std::shared_ptr<eServerMissile>& m);
    void summon(eServerUnit& by,
                ePointF to,
                const int unitId,
                const int maxCount,
                const std::vector<eModifier>& mods);

    ePointF emptyPlaceNear(const ePointF& pos) const;

    std::shared_ptr<eServerUnit>
    unit(const int charId) const;
    std::shared_ptr<eGroundItem>
    groundItem(const int itemId) const;
    using eValidator = std::function<bool(const eServerUnit&)>;
    std::shared_ptr<eServerUnit> unit(
        const ePointF& pos, const eValidator& validator) const;
    using eUnitIter = std::function<bool(const std::shared_ptr<eServerUnit>&)>;
    bool iterateOverUnits(const eArea& areaMin,
                          const eArea& areaMax,
                          const eUnitIter& iter) const;
    bool iterateOverUnits(const ePointF& pos,
                          const float maxRadius,
                          const eUnitIter& iter) const;

    void unitKilled(const eServerUnit& killed);
private:
    void removePlannedUnits();
    void iniSetupUnit(const std::shared_ptr<eServerUnit>& u,
                      const int charId,
                      const int teamId,
                      const ePointF& pos,
                      const eUnitInfo& uinfo,
                      const eCharData& data,
                      const eModelParts& modelParts);

    float mTime = 0.f;

    std::shared_ptr<eMap> mMap;

    static uint32_t sNextItemId;

    eIdMapVector<eServerMissile> mMissiles;
    eIdMapVector<eServerUnit> mUnits;
    eIdMapVector<eItem> mItemsOnGround;
    eIdMapVector<eGroundItem> mGroundItems;

    const int mUnitAreaDim = 4;
    const int mUnitAreaMargin = 3;
    eSetAreas mUnitAreas;
    const int mItemAreaDim = 4;
    eSetAreas mItemAreas;
    const int mItemTileSubdivision = 2;
    eSetAreas mItemTiles;

    std::vector<int> mUnitsToRemove;
    std::map<int, eClientData> mClientData;

    eMissileIncrementer mMIncrementer;
};

#endif // ESERVERAREA_H

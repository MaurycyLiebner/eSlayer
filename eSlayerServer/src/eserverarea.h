#ifndef ESERVERAREA_H
#define ESERVERAREA_H

#include "eserverunit.h"
#include "eservermissile.h"

#include <eSlayerMissiles/emissileincrementer.h>

#include <eSlayerHelpers/eunitdynamicdata.h>
#include <eSlayerHelpers/emovementhandler.h>
#include <eSlayerHelpers/eidmapvector.h>
#include <eSlayerHelpers/eunitarea.h>
#include <eSlayerHelpers/eunitareas.h>
#include <eSlayerHelpers/escreendimensions.h>

#include <eSlayerMapGenerator/emapgenerator.h>

#include <memory>
#include <map>
#include <set>

struct eClientData {
    // in tile dimensions
    eScreenDimensions fScreen;
    eUnitArea fArea;
    std::set<int> fKnownUnits;
    std::set<int> fKnownItems;
    int32_t fLatestMissile;
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

    eUnitArea unitArea(const int charId) const;
    eUnitArea unitArea(const eServerUnit& u) const;

    eUnitArea itemArea(const int itemId) const;
    eUnitArea itemArea(const eGroundItem& i) const;

    eUnitArea itemTile(const int itemId) const;
    eUnitArea itemTile(const eGroundItem& i) const;

    bool addClient(const int clientId,
                   const eEquipment& eq,
                   const ePointF& pos,
                   const eScreenDimensions& screenDims);
    bool removeClient(const int clientId);
    bool removeUnit(const int charId);

    bool pickupItem(const int clientId, const int itemId,
                    const bool drag);
    bool dropItem(const int clientId, const int itemId);
    void rearrangeItems(const int clientId, const eEquipment& eq);

    void addMissile(const std::shared_ptr<eServerMissile>& m);

    std::shared_ptr<eServerUnit>
    unit(const int charId) const;
    std::shared_ptr<eGroundItem>
    groundItem(const int itemId) const;
    std::shared_ptr<eServerUnit>
    unit(const ePointF& pos);

    void unitKilled(const eServerUnit& killed);
private:
    float mTime = 0.f;

    std::shared_ptr<eMap> mMap;

    eIdMapVector<eServerMissile> mMissiles;
    eIdMapVector<eServerUnit> mUnits;
    eIdMapVector<eItem> mItemsOnGround;
    eIdMapVector<eGroundItem> mGroundItems;

    const int mUnitAreaDim = 4;
    const int mUnitAreaMargin = 3;
    eUnitAreas mUnitAreas;
    const int mItemAreaDim = 4;
    eUnitAreas mItemAreas;
    const int mItemTileSubdivision = 2;
    eUnitAreas mItemTiles;

    std::map<int, eClientData> mClientData;

    eMissileIncrementer mMIncrementer;
};

#endif // ESERVERAREA_H

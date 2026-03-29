#ifndef ESERVERAREA_H
#define ESERVERAREA_H

#include "eserverunit.h"
#include "eservermissile.h"

#include <eSlayerHelpers/eunitdynamicdata.h>

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

    void unitsData(const int clientId,
                   std::vector<eUnitData>& newUnits,
                   std::vector<eUnitDynamicData>& updatedUnits);
    void itemsData(const int clientId,
                   std::vector<eGroundItem>& newItems,
                   std::vector<uint32_t>& removedItemIds);

    eUnitArea unitArea(const int charId) const;
    eUnitArea unitArea(const eServerUnit& u) const;

    bool addClient(const int clientId,
                   const eEquipment& eq,
                   const ePointF& pos);
    bool removeClient(const int clientId);
    bool removeUnit(const int charId);

    bool pickupItem(const int clientId, const int itemId,
                    const bool drag);
    bool dropItem(const int clientId, const int itemId);
    void rearrangeItems(const int clientId, const eEquipment& eq);

    std::vector<eMissile>
    missileData(const int clientId) const;

    void addMissile(const std::shared_ptr<eServerMissile>& m);
    void removeMissile(const std::shared_ptr<eServerMissile>& m);

    std::shared_ptr<eServerUnit>
    unit(const int charId) const;
    std::shared_ptr<eGroundItem>
    groundItem(const int itemId) const;
    std::shared_ptr<eServerUnit>
    unit(const ePointF& pos);
private:
    float mTime = 0.f;

    eIdMapVector<eServerMissile> mMissiles;
    mutable std::map<int, int32_t> mClientLatestMissileId;
    eIdMapVector<eServerUnit> mUnits;
    eIdMapVector<eItem> mItemsOnGround;
    eIdMapVector<eGroundItem> mGroundItems;
    const int mUnitAreaDim = 4;
    const int mUnitAreaMargin = 3;
    eUnitAreas mUnitAreas;
    const int mItemAreaDim = -2;
    const int mItemAreaMargin = 6;
    eUnitAreas mItemAreas;
    std::map<int, eUnitArea> mClientAreas;
    std::set<int> mClientIds;
    std::map<int, std::set<int>> mClientKnownUnits;
    std::map<int, std::set<int>> mClientKnownItems;

    std::shared_ptr<eMap> mMap;
};

#endif // ESERVERAREA_H

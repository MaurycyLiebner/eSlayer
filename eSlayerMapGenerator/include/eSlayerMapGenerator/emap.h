#ifndef EMAP_H
#define EMAP_H

#include "eslayermapgeneratorexport.h"

#include "echamber.h"

#include <eSlayerHelpers/emapsettings.h>
#include <eSlayerHelpers/emapportion.h>
#include <eSlayerHelpers/epathfindermap.h>
#include <eSlayerHelpers/eobstaclesmap.h>
#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/erect.h>
#include <eSlayerHelpers/estringidmapvector.h>
#include <eSlayerHelpers/edoors.h>
#include <eSlayerHelpers/ecamprects.h>

#include <memory>
#include <vector>
#include <optional>

class ePacket;

enum class eMoveToMapType {
    spawn,
    respawn, entrance,
    waypoint, portal
};

struct eMoveToMapData {
    eMoveToMapType fType;
    eAreaIds fFrom;
    eAreaIds fTo;
    uint32_t fPortalId;
};

class ESLAYERMAPGENERATOR_API eMap {
    friend class eMapGenerator;
    friend class eWallFinisher;
    friend class eDungeon;
    friend class eServer;
public:
    eMap();
    eMap(const uint8_t id);

    int width() const { return mWidth; }
    int height() const { return mHeight; }

    uint8_t id() const { return mId; }
    uint8_t actId() const;

    const eTile& tile(const int x, const int y) const;
    eTile& tile(const int x, const int y);
    const std::vector<int>& objects(const int x, const int y) const;
    std::shared_ptr<eObject> object(const int x, const int y,
                                    const uint32_t objectId) const;
    std::shared_ptr<eObject> object(const ePointF& pos,
                                    const uint32_t objectId) const;
    const std::shared_ptr<eObject>& object(const int id) const;

    const ePointF& spawnPos() const { return mSpawnPos; }
    bool spawnPos(const eAreaIds& from,
                  ePointF& result) const;
    const ePointF& portalSpawnPos() const { return mPortalSpawnPos; }

    const std::set<uint16_t>&
    terrainTypes() const { return mTerrainTypes; }

    const std::set<uint16_t>&
    objectTypes() const { return mObjectTypes; }

    const std::set<uint16_t>&
    unitTypes() const { return mUnitTypes; }

    bool walkable(const ePointF& pos);
    bool walkable(const ePointF& from, const ePointF& to);
    bool obstacle(const ePointF& pos);
    bool obstacle(const ePointF& from, const ePointF& to);
    bool hasObjects(const int x, const int y) const;
    bool inside(const int x, const int y) const;;

    bool hasPosPortion(const ePointF& pos);
    bool addObjectIfHasPortion(
        const std::shared_ptr<eObject>& o);
    bool objectPosition(const uint32_t objectId,
                        ePointF& pos) const;
    void addObject(const std::shared_ptr<eObject>& o);
    void removeObject(const uint32_t objectId);
    std::shared_ptr<eObject> addObject(
        const ePointF& pos,
        const float width,
        const float height);

    void loadPortion(const eMapPortion& portion);
    bool extractPortion(eMapPortionArea area, eMapPortion& result) const;
    void mapData(eMapData& data) const;
    void loadData(const eMapData& data);
    void setSpawnPos(const ePointF& pos);
    bool hasPortion(const int x, const int y);

    int areaAt(const ePointF& pos) const;
    int areaAt(const ePoint& pos) const;
    std::string areaName(const int id);
    eMapArea& area(const int id);

    struct eMonsterArea {
        uint8_t fLevel;
        std::vector<eChamber> fChambers;
        eAreaMonsterSettings fSettings;
    };

    const std::vector<eMonsterArea>& monsterAreas() const
    { return mMonsterAreas; }

    struct eBlueprintUnit {
        ePointF fPos;
        uint16_t fType;
        std::set<uint8_t> fElite;
        int fCount;
        std::vector<eItemDrop> fItemDrops;
        uint8_t fLevel;
    };

    const std::vector<eBlueprintUnit>& blueprintUnits() const
    { return mBlueprintUnits; }

    ePathFinderMap& pathFinderMap() { return mPathFinderMap; }
    void fillPathFinderMap();
    void triggerDoors(const eDoors& doors);

    std::optional<eMapStairs> mapStairs(
        const int x, const int y,
        const eWallType type) const;
    void addStairs(const int x, const int y,
                   const eWallType wallType,
                   const eConnectionDir dir,
                   const uint8_t type,
                   const eAreaIds& to);

    bool waypointPosition(
        const uint8_t areaId, ePointF& pos) const;

    bool campAt(const ePointF& pos) const;
    bool campAtLine(const ePointF& from,
                    const ePointF& to) const;
    void addCampRect(const eRectF& rect);
private:
    void generateTiles(const int w, const int h);
    using eIter = std::function<void(const int x, const int y)>;
    void iterateOverObjectTiles(const std::shared_ptr<eObject>& o,
                                const eIter& iter);

    uint8_t mId = 0;

    eCampRects mCamp;

    ePointF mSpawnPos{0, 0};
    ePointF mPortalSpawnPos{0, 0};

    uint16_t mWidth = 0;
    uint16_t mHeight = 0;

    eStringIdMapVector<eMapArea> mAreas;

    bool mAllPresent = false;
    std::vector<std::vector<bool>> mPresent;

    std::vector<std::vector<eTile>> mTiles;
    std::vector<std::shared_ptr<eObject>> mObjects;
    std::vector<std::vector<std::vector<int>>> mObjectsMap;

    std::set<uint16_t> mTerrainTypes;
    std::set<uint16_t> mObjectTypes;
    std::set<uint16_t> mUnitTypes;

    std::vector<eMonsterArea> mMonsterAreas;
    std::vector<eBlueprintUnit> mBlueprintUnits;

    ePathFinderMap mPathFinderMap;
    eObstaclesMap mObstaclesMap;

    std::vector<eMapStairs> mStairs;
    std::vector<std::shared_ptr<eObject>> mConnObjs;
};

#endif // EMAP_H

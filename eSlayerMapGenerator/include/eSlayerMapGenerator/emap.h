#ifndef EMAP_H
#define EMAP_H

#include "emapsettings.h"
#include "eslayermapgeneratorexport.h"

#include <eSlayerHelpers/emapportion.h>
#include <eSlayerHelpers/epathfindermap.h>
#include <eSlayerHelpers/eobstaclesmap.h>
#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/erect.h>
#include <eSlayerHelpers/estringidmapvector.h>
#include <eSlayerHelpers/edoors.h>

#include <memory>
#include <vector>
#include <optional>

class ePacket;

class ESLAYERMAPGENERATOR_API eMap {
    friend class eMapGenerator;
    friend class eDungeon;
    friend class eServer;
public:
    int width() const { return mWidth; }
    int height() const { return mHeight; }

    const eTile& tile(const int x, const int y) const;
    eTile& tile(const int x, const int y);
    const std::vector<int>& objects(const int x, const int y) const;
    std::shared_ptr<eObject> object(const int x, const int y,
                                    const uint32_t objectId) const;
    std::shared_ptr<eObject> object(const ePointF& pos,
                                    const uint32_t objectId) const;
    const std::shared_ptr<eObject>& object(const int id) const;

    const ePoint& spawnPos() const { return mSpawnPos; }

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

    void loadPortion(const eMapPortion& portion);
    bool extractPortion(eMapPortionArea area, eMapPortion& result) const;
    void mapData(eMapData& data) const;
    void loadData(const eMapData& data);
    bool hasPortion(const int x, const int y);

    int areaAt(const ePoint& pos) const;
    std::string areaName(const int id);
    eMapArea& area(const int id);

    struct eMonsterArea {
        eRect fRect;
        eMapMonsterSettings fSettings;
    };

    const std::vector<eMonsterArea>& monsterAreas() const
    { return mMonsterAreas; }

    ePathFinderMap& pathFinderMap() { return mPathFinderMap; }
    void fillPathFinderMap();
    void triggerDoors(const eDoors& doors);

    std::optional<int> stairsMapId(
        const int x, const int y,
        const eWallType type) const;
    void addStairs(const int x, const int y,
                   const eWallType wallType,
                   const eConnectionDir dir,
                   const uint8_t type,
                   const int mapId);
private:
    void generateTiles(const int w, const int h);
    void updateObjectsMap();
    const std::shared_ptr<eObject>& addObject();

    ePoint mSpawnPos{0, 0};

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

    ePathFinderMap mPathFinderMap;
    eObstaclesMap mObstaclesMap;

    std::vector<eMapStairs> mStairs;
};

#endif // EMAP_H

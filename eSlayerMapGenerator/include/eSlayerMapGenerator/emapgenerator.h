#ifndef EMAPGENERATOR_H
#define EMAPGENERATOR_H

#include "eslayermapgeneratorexport.h"

#include <eSlayerHelpers/epoint.h>
#include <eSlayerHelpers/emapportion.h>
#include <eSlayerHelpers/erect.h>
#include <eSlayerHelpers/estringidmapvector.h>

#include <string>
#include <memory>
#include <vector>

class ePacket;

class ESLAYERMAPGENERATOR_API eMap {
    friend class eMapGenerator;
    friend class eServer;
public:
    int width() const { return mWidth; }
    int height() const { return mHeight; }

    const eTile& tile(const int x, const int y) const;
    const std::vector<uint16_t>& objects(const int x, const int y) const;
    const eObject& object(const int id) const;

    const ePoint& spawnPos() const { return mSpawnPos; }

    const std::set<uint16_t>&
    terrainTypes() const { return mTerrainTypes; }

    const std::set<uint16_t>&
    objectTypes() const { return mObjectTypes; }

    const std::set<uint16_t>&
    unitTypes() const { return mUnitTypes; }

    bool walkable(const int x, const int y) const;

    void write(ePacket& p) const;
    void read(ePacket& p);

    void loadPortion(const eMapPortion& portion);
    bool extractPortion(eMapPortionArea area, eMapPortion& result) const;
    void mapData(eMapData& data) const;
    void loadData(const eMapData& data);

    int areaAt(const ePoint& pos) const;
    std::string areaName(const int id);
    eMapArea& area(const int id);
private:
    void generateTiles(const int w, const int h);
    void updateObjectsMap();

    ePoint mSpawnPos{0, 0};

    uint16_t mWidth = 0;
    uint16_t mHeight = 0;

    eStringIdMapVector<eMapArea> mAreas;

    std::vector<std::vector<eTile>> mTiles;
    std::vector<eObject> mObjects;
    std::vector<std::vector<std::vector<uint16_t>>> mObjectsMap;
    std::set<uint16_t> mTerrainTypes;
    std::set<uint16_t> mObjectTypes;
    std::set<uint16_t> mUnitTypes;
};

namespace eSlayerMapGenerator {
    ESLAYERMAPGENERATOR_API std::shared_ptr<eMap>
    generate(const std::string& name);
}

#endif // EMAPGENERATOR_H

#ifndef EMAPGENERATOR_H
#define EMAPGENERATOR_H

#include "eslayermapgeneratorexport.h"

#include <eSlayerHelpers/epoint.h>

#include <string>
#include <memory>
#include <vector>

struct ESLAYERMAPGENERATOR_API eTile {
    uint16_t fTerrainType;
    uint16_t fTileType;
};

struct ESLAYERMAPGENERATOR_API eObject {
    uint16_t fObjectType;
    uint16_t fTileType;

    uint16_t fTileX;
    uint16_t fTileY;
};

class ePacket;

class ESLAYERMAPGENERATOR_API eMap {
    friend class eMapGenerator;
    friend class eMapPortion;
public:
    int width() const { return mWidth; }
    int height() const { return mHeight; }

    const eTile& tile(const int x, const int y) const;
    const std::vector<uint16_t>& objects(const int x, const int y) const;
    const eObject& object(const int id) const;

    const std::vector<uint16_t>&
    terrainTypes() const { return mTerrainTypes; }

    const std::vector<uint16_t>&
    objectTypes() const { return mObjectTypes; }

    bool walkable(const int x, const int y) const;

    void write(ePacket& p) const;
    void read(ePacket& p);
private:
    void updateObjectsMap();

    ePointF mSpawnPos{0.f, 0.f};
    uint16_t mWidth = 0;
    uint16_t mHeight = 0;
    std::vector<std::vector<eTile>> mTiles;
    std::vector<eObject> mObjects;
    std::vector<std::vector<std::vector<uint16_t>>> mObjectsMap;
    std::vector<uint16_t> mTerrainTypes;
    std::vector<uint16_t> mObjectTypes;
};

namespace eSlayerMapGenerator {
    ESLAYERMAPGENERATOR_API std::shared_ptr<eMap>
    generate(const std::string& name);
}

#endif // EMAPGENERATOR_H

#ifndef EMAPGENERATOR_H
#define EMAPGENERATOR_H

#include "eslayermapgeneratorexport.h"

#include <string>
#include <memory>
#include <vector>

struct ESLAYERMAPGENERATOR_API eTerrainType {
    std::string fName;
};

struct ESLAYERMAPGENERATOR_API eObjectType {
    std::string fName;
};

struct ESLAYERMAPGENERATOR_API eTile {
    int fTerrainType;
    int fTileType;
};

struct ESLAYERMAPGENERATOR_API eObject {
    int fObjectType;
    int fTileType;

    int fTileX;
    int fTileY;
};

class ESLAYERMAPGENERATOR_API eMap {
    friend class eMapGenerator;
  public:
    int width() const { return mWidth; }
    int height() const { return mHeight; }

    const eTile& tile(const int x, const int y) const;
    const std::vector<int>& objects(const int x, const int y) const;
    const eObject& object(const int id) const;

    const std::vector<eTerrainType>&
    terrainTypes() const { return mTerrainTypes; }

    const std::vector<eObjectType>&
    objectTypes() const { return mObjectTypes; }

    bool walkable(const int x, const int y) const;
  private:
    void updateObjectsMap();

    int mWidth = 0;
    int mHeight = 0;
    std::vector<std::vector<eTile>> mTiles;
    std::vector<eObject> mObjects;
    std::vector<std::vector<std::vector<int>>> mObjectsMap;
    std::vector<eTerrainType> mTerrainTypes;
    std::vector<eObjectType> mObjectTypes;
};

namespace eSlayerMapGenerator {
    ESLAYERMAPGENERATOR_API std::shared_ptr<eMap>
    generate(const std::string& name);
}

#endif // EMAPGENERATOR_H

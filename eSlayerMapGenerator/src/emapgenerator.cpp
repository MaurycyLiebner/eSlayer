#include "../include/eSlayerMapGenerator/emapgenerator.h"

class eMapGenerator {
public:
    eMapGenerator() {}

    std::shared_ptr<eMap>
    generate(const std::string& name) const;
};

std::shared_ptr<eMap>
eSlayerMapGenerator::generate(const std::string& name) {
    eMapGenerator g;
    return g.generate(name);
}

const eTile& eMap::tile(const int x, const int y) const {
    return mTiles[y][x];
}

const std::vector<int>& eMap::objects(const int x, const int y) const {
    return mObjectsMap[y][x];
}

const eObject& eMap::object(const int id) const {
    return mObjects[id];
}

bool eMap::walkable(const int x, const int y) const {
    if(x < 0 || x >= mWidth ||
       y < 0 || y >= mHeight) {
        return false;
    } else {
        const auto& objs = objects(x, y);
        return objs.empty();
    }
}

void eMap::updateObjectsMap() {
    mObjectsMap.clear();

    for(int y = 0; y < mHeight; y++) {
        auto& row = mObjectsMap.emplace_back();
        row.reserve(mWidth);
        for(int x = 0; x < mWidth; x++) {
            row.emplace_back();
        }
    }

    const int iMax = mObjects.size();
    for(int i = 0; i < iMax; i++) {
        const auto& o = mObjects[i];
        mObjectsMap[o.fTileY][o.fTileX].emplace_back(i);
    }
}

std::shared_ptr<eMap>
eMapGenerator::generate(const std::string& name) const {
    const auto result = std::make_shared<eMap>();
    if(name == "town") {
        auto& terrType = result->mTerrainTypes.emplace_back();
        terrType.fName = "town_floor";
        const int w = 40;
        const int h = 80;
        for(int y = 0; y < h; y++) {
            auto& row = result->mTiles.emplace_back();
            row.reserve(w);
            for(int x = 0; x < w; x++) {
                row.emplace_back(eTile{0, 0});
            }
        }
        result->mWidth = w;
        result->mHeight = h;
        result->mTiles[0][0].fTileType = 3;
        result->mTiles[0][1].fTileType = 2;
        result->mTiles[1][0].fTileType = 1;

        auto& objType = result->mObjectTypes.emplace_back();
        objType.fName = "town_fence";

        auto& obj = result->mObjects.emplace_back();
        obj.fObjectType = 0;
        obj.fTileType = 29;
        obj.fTileX = 5;
        obj.fTileY = 5;

        for(int x = 8; x <= 9; x++) {
            for(int y = 8; y <= 9; y++) {
                auto& obj = result->mObjects.emplace_back();
                obj.fObjectType = 0;
                obj.fTileType = 29;
                obj.fTileX = x;
                obj.fTileY = y;
            }
        }

        for(int x = 11; x <= 12; x++) {
            for(int y = 11; y <= 12; y++) {
                auto& obj = result->mObjects.emplace_back();
                obj.fObjectType = 0;
                obj.fTileType = 29;
                obj.fTileX = x;
                obj.fTileY = y;
            }
        }

        result->updateObjectsMap();
    }
    return result;
}

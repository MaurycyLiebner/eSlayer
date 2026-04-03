#include "eSlayerMapGenerator/emapgenerator.h"

#include <eSlayerHelpers/epacket.h>
#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/eobjstexturesdata.h>

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

const std::vector<uint16_t>& eMap::objects(const int x, const int y) const {
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

void eMap::write(ePacket& p) const {
    const uint16_t nTerrTypes = mTerrainTypes.size();
    p << nTerrTypes;
    for(const auto& terrType : mTerrainTypes) {
        p << terrType;
    }

    p << mWidth;
    p << mHeight;
    for(int y = 0; y < mHeight; y++) {
        for(int x = 0; x < mWidth; x++) {
            const auto& tile = mTiles[y][x];
            p << tile.fTerrainType;
            p << tile.fTileType;
        }
    }

    const uint16_t nObjTypes = mObjectTypes.size();
    p << nObjTypes;
    for(const auto& objType : mObjectTypes) {
        p << objType;
    }

    const uint16_t nObjs = mObjects.size();
    p << nObjs;
    for(const auto& obj : mObjects) {
        p << obj.fObjectType;
        p << obj.fTileType;
        p << obj.fTileX;
        p << obj.fTileY;
    }
}

void eMap::read(ePacket& p) {
    uint16_t nTerrTypes;
    p >> nTerrTypes;
    for(int i = 0; i < nTerrTypes; i++) {
        auto& terrType = mTerrainTypes.emplace_back();
        p >> terrType;
    }

    p >> mWidth;
    p >> mHeight;
    mTiles.reserve(mHeight);
    for(uint16_t y = 0; y < mHeight; y++) {
        auto& row = mTiles.emplace_back();
        row.reserve(mWidth);
        for(int x = 0; x < mWidth; x++) {
            auto& tile = row.emplace_back();
            p >> tile.fTerrainType;
            p >> tile.fTileType;
        }
    }

    uint16_t nObjTypes;
    p >> nObjTypes;
    for(uint16_t i = 0; i < nObjTypes; i++) {
        auto& objType = mObjectTypes.emplace_back();
        p >> objType;
    }

    uint16_t nObjs;
    p >> nObjs;
    for(uint16_t i = 0; i < nObjs; i++) {
        auto& obj = mObjects.emplace_back();
        p >> obj.fObjectType;
        p >> obj.fTileType;
        p >> obj.fTileX;
        p >> obj.fTileY;
    }

    updateObjectsMap();
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
        const auto townFloorId = eTerrsTexturesData::id("town_floor");
        result->mTerrainTypes.emplace_back(townFloorId);
        const int w = 80;
        const int h = 80;
        result->mTiles.reserve(h);
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

        const auto townFenceId = eObjsTexturesData::id("town_fence");
        result->mObjectTypes.emplace_back(townFenceId);

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

#include "eSlayerMapGenerator/emapgenerator.h"

#include <eSlayerHelpers/epacket.h>
#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/eobjstexturesdata.h>
#include <eSlayerHelpers/echardatainfo.h>

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

    const uint16_t nUnitTypes = mUnitTypes.size();
    p << nUnitTypes;
    for(const auto& unitType : mUnitTypes) {
        p << unitType;
    }
}

void eMap::read(ePacket& p) {
    uint16_t nTerrTypes;
    p >> nTerrTypes;
    for(int i = 0; i < nTerrTypes; i++) {
        uint16_t terrType;
        p >> terrType;
        mTerrainTypes.emplace(terrType);
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
        uint16_t objType;
        p >> objType;
        mObjectTypes.emplace(objType);
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

    uint16_t nUnitTypes;
    p >> nUnitTypes;
    for(uint16_t i = 0; i < nUnitTypes; i++) {
        uint16_t unitType;
        p >> unitType;
        mUnitTypes.emplace(unitType);
    }
}

void eMap::loadPortion(const eMapPortion& portion) {
    const auto& area = portion.fArea;

    for(uint16_t y = 0; y < area.fHeight; y++) {
        for(uint16_t x = 0; x < area.fWidth; x++) {
            const auto& srcTile = portion.fTiles[y][x];
            auto& dstTile = mTiles[y + area.fY][x + area.fX];
            dstTile = srcTile;
        }
    }

    for(const auto& o : portion.fObjects) {
        const int i = mObjects.size();
        mObjects.emplace_back(o);
        mObjectsMap[o.fTileY][o.fTileX].emplace_back(i);
    }
}


void generateTiles(const int w, const int h,
                   std::vector<std::vector<eTile>>& tiles) {
    tiles.clear();
    tiles.reserve(h);
    for(int y = 0; y < h; y++) {
        auto& row = tiles.emplace_back();
        row.reserve(w);
        for(int x = 0; x < w; x++) {
            row.emplace_back(eTile{0, 0});
        }
    }
}

bool eMap::extractPortion(
    eMapPortionArea area,
    eMapPortion& result) const {
    if(area.fX < 0) {
        area.fWidth += area.fX;
        area.fX = 0;
    }
    if(area.fY < 0) {
        area.fHeight += area.fY;
        area.fY = 0;
    }
    if(area.fX + area.fWidth > mWidth) {
        area.fWidth = mWidth - area.fX;
    }
    if(area.fY + area.fHeight > mHeight) {
        area.fHeight = mHeight - area.fY;
    }

    if(area.fWidth <= 0 || area.fHeight <= 0) {
        return false;
    }

    result.fArea = area;

    ::generateTiles(area.fWidth, area.fHeight, result.fTiles);
    for(uint16_t y = 0; y < area.fHeight; y++) {
        for(uint16_t x = 0; x < area.fWidth; x++) {
            const auto srcY = y + area.fY;
            const auto srcX = x + area.fX;
            const auto& srcTile = mTiles[srcY][srcX];
            auto& dstTile = result.fTiles[y][x];
            dstTile = srcTile;

            const auto& objs = objects(srcX, srcY);
            for(const auto id : objs) {
                const auto& o = object(id);
                result.fObjects.emplace_back(o);
            }
        }
    }
    return true;
}

void eMap::mapData(eMapData& data) const {
    data.fTotalWidth = mWidth;
    data.fTotalHeight = mHeight;
    data.fObjectTypes = mObjectTypes;
    data.fTerrainTypes = mTerrainTypes;
    data.fUnitTypes = mUnitTypes;
}

void eMap::loadData(const eMapData& data) {
    mWidth = data.fTotalWidth;
    mHeight = data.fTotalHeight;
    generateTiles(mWidth, mHeight);
    mTerrainTypes = data.fTerrainTypes;
    mObjectTypes = data.fObjectTypes;
    mUnitTypes = data.fUnitTypes;
    updateObjectsMap();
}

void eMap::generateTiles(const int w, const int h) {
    ::generateTiles(w, h, mTiles);
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
        const uint16_t townFloorId = eTerrsTexturesData::id("grass");
        result->mTerrainTypes.emplace(townFloorId);
        const int w = 80;
        const int h = 80;
        result->generateTiles(w, h);
        for(auto& row : result->mTiles) {
            for(auto& tile : row) {
                tile.fTerrainType = 1;
                tile.fTileType = eRand::rand() % 20;
            }
        }
        result->mWidth = w;
        result->mHeight = h;

        const auto townFenceId = eObjsTexturesData::id("town_fence");
        result->mObjectTypes.emplace(townFenceId);

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

        for(const auto& unitName : {"wendigo", "mummy"}) {
            const auto unitId = eCharDataInfo::id(unitName);
            result->mUnitTypes.emplace(unitId);
        }
    }
    return result;
}

#include "eSlayerMapGenerator/emap.h"

#include <eSlayerHelpers/epacket.h>
#include <eSlayerHelpers/erect.h>
#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/eobjectsinfo.h>

uint32_t sNextObjectId = 1;

const eTile& eMap::tile(const int x, const int y) const {
    return mTiles[y][x];
}

eTile& eMap::tile(const int x, const int y) {
    return mTiles[y][x];
}

const std::vector<int>& eMap::objects(
    const int x, const int y) const {
    return mObjectsMap[y][x];
}

std::shared_ptr<eObject>
eMap::object(const int x, const int y,
             const uint32_t objectId) const {
    const auto& objIds = objects(x, y);
    for(const auto objId : objIds) {
        const auto& obj = object(objId);
        if(obj->fObjectId == objectId) return obj;
    }
    return nullptr;
}

std::shared_ptr<eObject> eMap::object(
    const ePointF& pos, const uint32_t objectId) const {
    const auto ipos = pos.floor();
    return object(ipos.fX, ipos.fY, objectId);
}

const std::shared_ptr<eObject>& eMap::object(
    const int id) const {
    return mObjects[id];
}

bool inside(const ePointF& pos,
            const float x,
            const float y,
            const float width,
            const float height) {
    if(pos.fX < x) return false;
    if(pos.fY < y) return false;
    if(pos.fX >= x + width) return false;
    if(pos.fY >= y + height) return false;
    return true;
};

bool eMap::wallBase(const ePointF& pos,
                    const int x, const int y,
                    const eTile& tile) const {
    const auto terrType = tile.fTerrainType;
    const auto& info = eTerrsTexturesData::get(terrType);

    const float thick = info.fWallsThickness;

    if(tile.fWallTL) {
        bool inWalls = true;
        const bool doors = eTile::doors(tile.fWallTL);
        if(doors) {
            const bool open = eTile::open(tile.fWallTL);
            if(open) {
                const auto type = eTile::type(tile.fWallTL);
                const auto& wallInfo = info.fTLDoorsOpen[type];
                const float wallMin = wallInfo.fWallMin;
                const float wallMax = wallInfo.fWallMax;
                const float dy = pos.fY - y;
                inWalls = dy > wallMin && dy < wallMax;
            }
        }
        if(inWalls) {
            const bool r = ::inside(pos, x - thick, y - thick, 2*thick, 1.f + 2*thick);
            if(r) return true;
        }
    }

    if(tile.fWallTR) {
        bool inWalls = true;
        const bool doors = eTile::doors(tile.fWallTR);
        if(doors) {
            const bool open = eTile::open(tile.fWallTR);
            if(open) {
                const auto type = eTile::type(tile.fWallTR);
                const auto& wallInfo = info.fTRDoorsOpen[type];
                const float wallMin = wallInfo.fWallMin;
                const float wallMax = wallInfo.fWallMax;
                const float dx = pos.fX - x;
                inWalls = dx > wallMin && dx < wallMax;
            }
        }
        if(inWalls) {
            const bool r = ::inside(pos, x - thick, y - thick, 1.f + 2*thick, 2*thick);
            if(r) return true;
        }
    }

    return false;
}

bool eMap::wall(const ePointF& pos,
                const int x, const int y,
                const eTile& tile) const {
    const bool r = wallBase(pos, x, y, tile);
    if(r) return true;

    if(x - 1 >= 0) {
        const int tx = x - 1;
        const int ty = y;
        const auto& tile = eMap::tile(tx, ty);
        const bool r = wallBase(pos, tx, ty, tile);
        if(r) return true;
    }

    if(y - 1 >= 0) {
        const int tx = x;
        const int ty = y - 1;
        const auto& tile = eMap::tile(tx, ty);
        const bool r = wallBase(pos, tx, ty, tile);
        if(r) return true;
    }

    if(y + 1 < mHeight) {
        const int tx = x;
        const int ty = y + 1;
        const auto& tile = eMap::tile(tx, ty);
        const bool r = wallBase(pos, tx, ty, tile);
        if(r) return true;
    }

    if(x + 1 < mWidth) {
        const int tx = x + 1;
        const int ty = y;
        const auto& tile = eMap::tile(tx, ty);
        const bool r = wallBase(pos, tx, ty, tile);
        if(r) return true;
    }

    return false;
}

bool eMap::walkable(const ePointF& pos) const {
    const auto iPos = pos.floor();
    const int x = iPos.fX;
    const int y = iPos.fY;
    if(x < 0 || x >= mWidth ||
       y < 0 || y >= mHeight) {
        return false;
    } else {
        const auto& tile = eMap::tile(x, y);
        const auto terrType = tile.fTerrainType;
        if(terrType == 0) return false;
        const auto tileType = tile.fTileType;
        if(tileType == 0) return false;
        const auto& info = eTerrsTexturesData::get(terrType);
        const bool w = info.fWalkable[tileType];
        if(!w) return false;

        const bool wall = eMap::wall(pos, x, y, tile);
        if(wall) return false;

        const auto& objs = objects(x, y);
        const bool empty = objs.empty();
        if(empty) return true;
        for(const auto oid : objs) {
            const auto& o = *object(oid);
            const auto& opos = o.fPos;
            const bool r = ::inside(pos, opos.fX, opos.fY,
                                    o.fSize, o.fSize);
            if(r) return false;
        }
    }
    return true;
}

bool eMap::obsticle(const ePointF& pos) const {
    const auto iPos = pos.floor();
    const int x = iPos.fX;
    const int y = iPos.fY;
    if(x < 0 || x >= mWidth ||
       y < 0 || y >= mHeight) {
        return false;
    } else {
        const auto& tile = eMap::tile(x, y);
        const auto terrType = tile.fTerrainType;
        if(terrType == 0) return false;
        const auto tileType = tile.fTileType;
        if(tileType == 0) return false;
        const auto& info = eTerrsTexturesData::get(terrType);
        const bool o = info.fObsticle[tileType];
        if(o) return true;


        const bool wall = eMap::wall(pos, x, y, tile);
        if(wall) return true;

        const auto& objs = objects(x, y);
        const bool empty = objs.empty();
        if(empty) return false;
        for(const auto oid : objs) {
            const auto& o = *object(oid);
            const auto type = o.fObjectType;
            const auto& info = eObjectsInfo::sObjects.get(type);
            if(!info.fObsticle) continue;
            const auto& opos = o.fPos;
            const bool r = ::inside(pos, opos.fX, opos.fY,
                                    o.fSize, o.fSize);
            if(r) return true;
        }
    }
    return false;
}

bool eMap::hasObjects(const int x, const int y) const {
    if(!inside(x, y)) return false;
    return !objects(x, y).empty();
}

bool eMap::inside(const int x, const int y) const {
    return x >= 0 && x < mWidth &&
           y >= 0 && y < mHeight;
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
        const auto& pos = o->fPos;
        const auto iPos = pos.floor();
        mObjectsMap[iPos.fY][iPos.fX].emplace_back(i);
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
            row.emplace_back(eTile{0, 0, 0, 0});
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
    data.fSpawnPos = mSpawnPos;
    data.fAreas = mAreas;
}

void eMap::loadData(const eMapData& data) {
    generateTiles(data.fTotalWidth, data.fTotalHeight);
    mTerrainTypes = data.fTerrainTypes;
    mObjectTypes = data.fObjectTypes;
    mUnitTypes = data.fUnitTypes;
    mSpawnPos = data.fSpawnPos;
    mAreas = data.fAreas;
}

int eMap::areaAt(const ePoint& pos) const {
    for(const auto& it : mAreas) {
        const auto& a = it.fValue;
        if(a.fRect.contains(pos)) return it.fId;
    }
    return -1;
}

std::string eMap::areaName(const int id) {
    return mAreas.name(id);
}

eMapArea& eMap::area(const int id) {
    return mAreas.get(id);
}

void eMap::generateTiles(const int w, const int h) {
    ::generateTiles(w, h, mTiles);
    mWidth = w;
    mHeight = h;
    mObjectsMap.resize(mHeight, std::vector<std::vector<int>>(mWidth));
}

void eMap::updateObjectsMap() {
    const int iMax = mObjects.size();
    for(int i = 0; i < iMax; i++) {
        const auto& o = *mObjects[i];
        const auto iPos = o.fPos.floor();
        mObjectsMap[iPos.fY][iPos.fX].emplace_back(i);
    }
}

const std::shared_ptr<eObject>& eMap::addObject() {
    const auto& obj = mObjects.emplace_back(std::make_shared<eObject>());
    obj->fObjectId = sNextObjectId++;
    return obj;
}

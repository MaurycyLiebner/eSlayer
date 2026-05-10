#include "eSlayerMapGenerator/emap.h"

#include <eSlayerHelpers/epacket.h>
#include <eSlayerHelpers/erect.h>
#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/eobjectsinfo.h>

uint32_t sNextObjectId = 1;

const eTile& eMap::tile(const int x, const int y) const {
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

bool eMap::wall(const ePointF& pos,
                const int x, const int y,
                const eTile& tile) const {
    const float thick = 0.25f;

    if(tile.fWallTL) {
        const bool r = inside(pos, x - thick, y, 2*thick, 1.f);
        if(r) return true;
    }

    if(tile.fWallTR) {
        const bool r = inside(pos, x, y - thick, 1.f, 2*thick);
        if(r) return true;
    }

    if(y - 1 >= 0 && x - 1 >= 0) {
        const auto& tileX = eMap::tile(x - 1, y);
        const auto& tileY = eMap::tile(x, y - 1);
        const bool wallT = tileX.fWallTR || tileY.fWallTL;
        if(wallT) {
            const bool r = inside(pos, x, y, thick, thick);
            if(r) return true;
        }
    }

    if(y + 1 < mHeight) {
        const auto& tile = eMap::tile(x, y + 1);
        const bool wallBL = tile.fWallTR;
        if(wallBL) {
            const bool r = inside(pos, x, y + 1 - thick, 1.f, 2*thick);
            if(r) return true;
        }
    }

    if(x + 1 < mWidth) {
        const auto& tile = eMap::tile(x + 1, y);
        const bool wallBR = tile.fWallTL;
        if(wallBR) {
            const bool r = inside(pos, x + 1 - thick, y, 2*thick, 1.f);
            if(r) return true;
        }
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
        const auto tileType = tile.fTileType;
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
            const bool r = inside(pos, opos.fX, opos.fY,
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
        const auto tileType = tile.fTileType;
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
            const bool r = inside(pos, opos.fX, opos.fY,
                                  o.fSize, o.fSize);
            if(r) return true;
        }
    }
    return false;
}

bool eMap::hasObjects(const int x, const int y) const {
    if(x < 0 || x >= mWidth ||
       y < 0 || y >= mHeight) {
        return false;
    }
    return !objects(x, y).empty();
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

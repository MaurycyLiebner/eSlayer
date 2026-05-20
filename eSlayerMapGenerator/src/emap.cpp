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

bool eMap::walkable(const ePointF& pos) {
    return mObsticlesMap.walkable(pos);
}

bool eMap::walkable(const ePointF& from, const ePointF& to) {
    return mObsticlesMap.walkable(from, to);
}

bool eMap::obsticle(const ePointF& pos) {
    return mObsticlesMap.obsticle(pos);
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

void eMap::fillPathFinderMap() {
    mObsticlesMap.fillAll();
    const int sw = mWidth*ePathFinderMap::sSubdivide;
    const int sh = mHeight*ePathFinderMap::sSubdivide;
    for(int x = 0; x < sw; x++) {
        for(int y = 0; y < sh; y++) {
            for(int dx = -1; dx <= 1; dx++) {
                for(int dy = -1; dy <= 1; dy++) {
                    mPathFinderMap.walkable(ePoint{x, y}, dx, dy);
                }
            }
        }
    }
}

void eMap::triggerDoors(const eDoors& doors) {
    for(const auto& t : doors.fTiles) {
        const bool r = inside(t.fX, t.fY);
        if(!r) continue;
        auto& tile = eMap::tile(t.fX, t.fY);
        switch(doors.fType) {
        case eWallType::topLeft: {
            eTile::setOpen(tile.fWallTL, !doors.fOpen);
        } break;
        case eWallType::topRight: {
            eTile::setOpen(tile.fWallTR, !doors.fOpen);
        } break;
        }

        {
            const int sxMin = t.fX*ePathFinderMap::sSubdivide;
            const int syMin = t.fY*ePathFinderMap::sSubdivide;
            const int sxMax = sxMin + ePathFinderMap::sSubdivide;
            const int syMax = syMin + ePathFinderMap::sSubdivide;
            for(int sx = sxMin; sx < sxMax; sx++) {
                for(int sy = syMin; sy < syMax; sy++) {
                    mPathFinderMap.erase(ePoint{sx, sy});
                }
            }
        }
        {
            const int xMin = t.fX/eObsticlesMap::sTileSize - 1;
            const int yMin = t.fY/eObsticlesMap::sTileSize - 1;
            const int xMax = xMin + 2;
            const int yMax = yMin + 2;
            for(int x = xMin; x < xMax; x++) {
                for(int y = yMin; y < yMax; y++) {
                    mObsticlesMap.eraseTile(x, y);
                }
            }
        }
    }
}

void eMap::generateTiles(const int w, const int h) {
    ::generateTiles(w, h, mTiles);

    mWidth = w;
    mHeight = h;

    mObjectsMap.resize(mHeight, std::vector<std::vector<int>>(mWidth));

    const auto walkable = [this](const ePointF& from, const ePointF& to) {
        return eMap::walkable(from, to);
    };
    mPathFinderMap.initialize(w, h, walkable);

    const auto filler = [&](std::vector<eRectF>& walk,
                            std::vector<eRectF>& missile,
                            const int sx, const int sy) {
        const int dim = eObsticlesMap::sTileSize;
        const int minX = sx*dim - 1;
        const int minY = sy*dim - 1;
        const int maxX = minX + dim + 2;
        const int maxY = minY + dim + 2;

        for(int x = minX; x < maxX; x++) {
            for(int y = minY; y < maxY; y++) {
                const bool r = eMap::inside(x, y);
                if(!r) continue;
                const auto& tile = eMap::tile(x, y);

                bool tileWalkable = true;
                const auto terrType = tile.fTerrainType;
                if(terrType == 0) tileWalkable = false;
                const auto tileType = tile.fTileType;
                if(tileType == 0) tileWalkable = false;
                const auto& info = eTerrsTexturesData::get(terrType);
                const float thick = info.fWallsThickness;
                if(tileWalkable) {
                    const bool w = info.fWalkable[tileType];
                    if(!w) tileWalkable = false;
                }

                bool missileObsticle = false;
                if(terrType != 0 && tileType != 0) {
                    missileObsticle = info.fObsticle[tileType];
                }

                if(tileWalkable) {
                    const auto& objIds = eMap::objects(x, y);
                    for(const int id : objIds) {
                        const auto& o = eMap::object(id);
                        const auto& pos = o->fPos;
                        const float size = o->fSize;
                        walk.emplace_back(
                            pos.fX, pos.fY, size, size);
                    }
                } else {
                    walk.emplace_back(
                        float(x), float(y), 1.f, 1.f);
                }

                if(!missileObsticle) {
                    const auto& objIds = eMap::objects(x, y);
                    for(const int id : objIds) {
                        const auto& o = *eMap::object(id);
                        const auto type = o.fObjectType;
                        const auto& info = eObjectsInfo::sObjects.get(type);
                        if(!info.fObsticle) continue;
                        const auto& pos = o.fPos;
                        const float size = o.fSize;
                        missile.emplace_back(
                            pos.fX, pos.fY, size, size);
                    }
                } else {
                    missile.emplace_back(
                        float(x), float(y), 1.f, 1.f);
                }

                if(tile.fWallTL) {
                    const bool doors = eTile::doors(tile.fWallTL);
                    const bool open = eTile::open(tile.fWallTL);
                    if(doors && open) {
                        const auto type = eTile::type(tile.fWallTL);
                        const auto& wallInfo = info.fTLDoorsOpen[type];
                        const float wallMin = wallInfo.fWallMin;
                        const float wallMax = wallInfo.fWallMax;
                        if(wallMin < wallMax) {
                            const auto& r = walk.emplace_back(
                                x - thick, y + wallMin,
                                2*thick, wallMax - wallMin);
                            missile.emplace_back(r);
                        }
                    } else {
                        const auto& r = walk.emplace_back(
                            x - thick, y - thick,
                            2*thick, 1.f + 2*thick);
                        missile.emplace_back(r);
                    }
                }

                if(tile.fWallTR) {
                    const bool doors = eTile::doors(tile.fWallTR);
                    const bool open = eTile::open(tile.fWallTR);
                    if(doors && open) {
                        const auto type = eTile::type(tile.fWallTR);
                        const auto& wallInfo = info.fTRDoorsOpen[type];
                        const float wallMin = wallInfo.fWallMin;
                        const float wallMax = wallInfo.fWallMax;
                        if(wallMin < wallMax) {
                            const auto& r = walk.emplace_back(
                                x + wallMin, y - thick,
                                wallMax - wallMin, 2*thick);
                            missile.emplace_back(r);
                        }
                    } else {
                        const auto& r = walk.emplace_back(
                            x - thick, y - thick,
                            1.f + 2*thick, 2*thick);
                        missile.emplace_back(r);
                    }
                }
            }
        }
    };
    mObsticlesMap.initialize(filler, w, h);
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

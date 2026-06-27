#include "eSlayerMapGenerator/emap.h"

#include <eSlayerHelpers/epacket.h>
#include <eSlayerHelpers/erect.h>
#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/eobjectsinfo.h>
#include <eSlayerHelpers/evectorhelpers.h>

#include <optional>
#include <unordered_set>
#include <atomic>

std::atomic<uint32_t> sNextObjectId = 1;

eMap::eMap(const uint8_t id) :
    mId(id) {}

uint8_t eMap::actId() const {
    const auto& info = eMapsSettings::sMaps.get(mId);
    return info.fActId;
}

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

ePointF eMap::spawnPos(const eAreaIds& from) const {
    for(const auto& s : mStairs) {
        if(s.fTo != from) continue;
        return {s.fX, s.fY};
    }
    for(const auto& o : mTrapDoors) {
        if(o->fTo != from) continue;
        return o->fPos;
    }
    return spawnPos();
}

std::vector<std::shared_ptr<eObject>>&
eMap::trapDoors() {
    return mTrapDoors;
}

bool eMap::walkable(const ePointF& pos) {
    return mObstaclesMap.walkable(pos);
}

bool eMap::walkable(const ePointF& from, const ePointF& to) {
    return mObstaclesMap.walkable(from, to);
}

bool eMap::obstacle(const ePointF& pos) {
    return mObstaclesMap.obstacle(pos);
}

bool eMap::obstacle(const ePointF& from,
                    const ePointF& to) {
    return mObstaclesMap.obstacle(from, to);
}

bool eMap::hasObjects(const int x, const int y) const {
    if(!inside(x, y)) return false;
    return !objects(x, y).empty();
}

bool eMap::inside(const int x, const int y) const {
    return x >= 0 && x < mWidth &&
           y >= 0 && y < mHeight;
}

bool eMap::hasPosPortion(const ePointF& pos) {
    const auto ipos = pos.floor();
    if(!inside(ipos.fX, ipos.fY)) return false;
    if(mAllPresent) return true;
    const int px = ipos.fX/eMapPortion::sBaseDim;
    const int py = ipos.fY/eMapPortion::sBaseDim;
    return mPresent[py][px];
}

bool eMap::addObjectIfHasPortion(
    const std::shared_ptr<eObject>& o) {
    const auto& pos = o->fPos;
    const bool r = hasPosPortion(pos);
    if(!r) return false;
    addObject(o);
    return true;
}

bool eMap::objectPosition(const uint32_t objectId,
                          ePointF& pos) const {
    for(int i = 0; i < mObjects.size(); i++) {
        const auto& o = mObjects[i];
        if(o->fObjectId != objectId) continue;
        pos = o->fPos;
        return true;
    }
    return false;
}

void eMap::addObject(const std::shared_ptr<eObject>& o) {
    const int i = mObjects.size();
    mObjects.emplace_back(o);
    const auto& pos = o->fPos;
    const auto iPos = pos.floor();
    mObjectsMap[iPos.fY][iPos.fX].emplace_back(i);
}

void eMap::removeObject(const uint32_t objectId) {
    for(int i = 0; i < mObjects.size(); i++) {
        const auto& o = mObjects[i];
        if(o->fObjectId != objectId) continue;
        const auto& pos = o->fPos;
        const auto iPos = pos.floor();
        auto& vec = mObjectsMap[iPos.fY][iPos.fX];
        eVectorHelpers::remove(vec, i);
        break;
    }
}

void eMap::loadPortion(const eMapPortion& portion) {
    const auto& area = portion.fArea;

    if(!mAllPresent) {
        const int px = area.fX/eMapPortion::sBaseDim;
        const int py = area.fY/eMapPortion::sBaseDim;
        mPresent[py][px] = true;
    }

    for(uint16_t y = 0; y < area.fHeight; y++) {
        for(uint16_t x = 0; x < area.fWidth; x++) {
            const auto& srcTile = portion.fTiles[y][x];
            auto& dstTile = mTiles[y + area.fY][x + area.fX];
            dstTile = srcTile;
        }
    }

    for(const auto& o : portion.fObjects) {
        addObject(o);
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
            row.emplace_back(eTile{0, 0, 0, 0, 0, 0});
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
    data.fTerrainTypes = mTerrainTypes;
    data.fObjectTypes = mObjectTypes;
    data.fUnitTypes = mUnitTypes;
    data.fSpawnPos = mSpawnPos;
    data.fAreas = mAreas;
    data.fStairs = mStairs;
}

void eMap::loadData(const eMapData& data) {
    generateTiles(data.fTotalWidth, data.fTotalHeight);
    mTerrainTypes = data.fTerrainTypes;
    mObjectTypes = data.fObjectTypes;
    mUnitTypes = data.fUnitTypes;
    mSpawnPos = data.fSpawnPos;
    mAreas = data.fAreas;
    mStairs = data.fStairs;
}

void eMap::setSpawnPos(const ePointF& pos) {
    mSpawnPos = pos;
}

bool eMap::hasPortion(const int x, const int y) {
    if(!inside(x, y)) return false;
    if(mAllPresent) return true;
    const int px = x/eMapPortion::sBaseDim;
    const int py = y/eMapPortion::sBaseDim;
    return mPresent[py][px];
}

int eMap::areaAt(const ePointF& pos) const {
    return areaAt(pos.floor());
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
    mObstaclesMap.fillAll();
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
            const int xMin = t.fX/eObstaclesMap::sTileSize - 1;
            const int yMin = t.fY/eObstaclesMap::sTileSize - 1;
            const int xMax = xMin + 2;
            const int yMax = yMin + 2;
            for(int x = xMin; x < xMax; x++) {
                for(int y = yMin; y < yMax; y++) {
                    mObstaclesMap.eraseTile(x, y);
                }
            }
        }
    }
}

std::optional<eMapStairs> eMap::mapStairs(
    const int x, const int y,
    const eWallType type) const {
    for(const auto& s : mStairs) {
        if(s.fX != x) continue;
        if(s.fY != y) continue;
        if(s.fWallType != type) continue;
        return s;
    }
    return std::nullopt;
}

void eMap::addStairs(
    const int x, const int y,
    const eWallType wallType,
    const eConnectionDir dir,
    const uint8_t type,
    const eAreaIds& to) {
    auto& tile = eMap::tile(x, y);
    const bool up = dir == eConnectionDir::up;
    switch(wallType) {
    case eWallType::topLeft:
        tile.fStairsTL = eTile::encodeStairs(true, up, type);
        break;
    case eWallType::topRight:
        tile.fStairsTR = eTile::encodeStairs(true, up, type);
        break;
    }
    auto& stairs = mStairs.emplace_back();
    stairs.fX = x;
    stairs.fY = y;
    stairs.fWallType = wallType;
    stairs.fStairsDir = dir;
    stairs.fTo = to;
}

bool eMap::waypointPosition(
    const uint8_t areaId, ePointF& pos) const {
    for(const auto& o : mObjects) {
        const auto type = o->fObjectType;
        const auto& info = eObjectsInfo::sObjects.get(type);
        if(info.fType != eObjectType::waypoint) continue;
        const auto& opos = o->fPos;
        const auto cAreaId = areaAt(opos);
        if(cAreaId != areaId) continue;
        pos = opos;
        return true;
    }
    return false;
}

class eWallTL {
public:
    eWallTL(const eMap& map) :
        mMap(map) {}

    void initialize(int minY, int maxY,
                    const int x0, const int y0) {
        const auto& tile0 = mMap.tile(x0, y0);
        const auto terrType0 = tile0.fTerrainType;
        const auto& info = eTerrsTexturesData::get(terrType0);
        const float thick = info.fWallsThickness;

        std::optional<float> minRectY;
        std::optional<float> maxRectY;
        const auto processTile = [&](const int y) {
            const auto& tile = mMap.tile(x0, y);
            const auto terrType = tile.fTerrainType;
            if(terrType != terrType0) return false;
            if(!tile.fWallTL) return false;
            const bool doors = eTile::doors(tile.fWallTL);
            const bool open = eTile::open(tile.fWallTL);
            if(doors && open) {
                const auto type = eTile::type(tile.fWallTL);
                const auto& wallInfo = info.fTLDoorsOpen[type];
                const float wallMin = wallInfo.fWallMin;
                const float wallMax = wallInfo.fWallMax;
                if(wallMin == 0.f && wallMax < 1.f) maxY = y;
                if(wallMax == 1.f && wallMin > 0.f) minY = y;
                if(wallMin < wallMax) {
                    const float minYV = y + wallMin;
                    if(!minRectY) {
                        minRectY = minYV;
                    } else {
                        minRectY = std::min(*minRectY, minYV);
                    }
                    const float maxYV = y + wallMax;
                    if(!maxRectY) {
                        maxRectY = maxYV;
                    } else {
                        maxRectY = std::max(*maxRectY, maxYV);
                    }
                }
            } else {
                const float minYV = y - thick;
                if(!minRectY) {
                    minRectY = minYV;
                } else {
                    minRectY = std::min(*minRectY, minYV);
                }
                const float maxYV = y + 1.f + thick;
                if(!maxRectY) {
                    maxRectY = maxYV;
                } else {
                    maxRectY = std::max(*maxRectY, maxYV);
                }
            }
            return true;
        };

        mMinY = y0;
        mMaxY = y0;

        for(int y = y0; y >= minY; y--) {
            const bool r = processTile(y);
            if(!r) break;
            mMinY = y;
        }

        for(int y = y0 + 1; y <= maxY; y++) {
            const bool r = processTile(y);
            if(!r) break;
            mMaxY = y;
        }
        if(minRectY && maxRectY) {
            mRect = eRectF{x0 - thick, *minRectY,
                           2*thick, *maxRectY - *minRectY};
        }
    }

    int minY() const { return mMinY; }
    int maxY() const { return mMaxY; }
    const eRectF& rect() const { return mRect; }
private:
    const eMap& mMap;
    int mMinY = 0;
    int mMaxY = 0;
    eRectF mRect{0.f, 0.f, 0.f, 0.f};
};

class eWallTR {
public:
    eWallTR(const eMap& map) :
        mMap(map) {}

    void initialize(int minX, int maxX,
                    const int x0, const int y0) {
        const auto& tile0 = mMap.tile(x0, y0);
        const auto terrType0 = tile0.fTerrainType;

        const auto& info = eTerrsTexturesData::get(terrType0);

        const float thick = info.fWallsThickness;

        std::optional<float> minRectX;
        std::optional<float> maxRectX;

        const auto processTile = [&](const int x) {
            const auto& tile = mMap.tile(x, y0);

            if(tile.fTerrainType != terrType0) return false;

            if(!tile.fWallTR) return false;

            const bool doors = eTile::doors(tile.fWallTR);
            const bool open  = eTile::open(tile.fWallTR);

            if(doors && open) {
                const auto type = eTile::type(tile.fWallTR);
                const auto& wallInfo = info.fTRDoorsOpen[type];

                const float wallMin = wallInfo.fWallMin;
                const float wallMax = wallInfo.fWallMax;

                if(wallMin == 0.f && wallMax < 1.f) maxX = x;
                if(wallMax == 1.f && wallMin > 0.f) minX = x;

                if(wallMin < wallMax) {
                    const float minXV = x + wallMin;

                    if(!minRectX) {
                        minRectX = minXV;
                    } else {
                        minRectX = std::min(*minRectX, minXV);
                    }

                    const float maxXV = x + wallMax;

                    if(!maxRectX) {
                        maxRectX = maxXV;
                    } else {
                        maxRectX = std::max(*maxRectX, maxXV);
                    }
                }
            } else {
                const float minXV = x - thick;

                if(!minRectX) {
                    minRectX = minXV;
                } else {
                    minRectX = std::min(*minRectX, minXV);
                }

                const float maxXV = x + 1.f + thick;

                if(!maxRectX) {
                    maxRectX = maxXV;
                } else {
                    maxRectX = std::max(*maxRectX, maxXV);
                }
            }

            return true;
        };

        mMinX = x0;
        mMaxX = x0;

        for(int x = x0; x >= minX; --x) {
            const bool r = processTile(x);
            if(!r) break;
            mMinX = x;
        }

        for(int x = x0 + 1; x <= maxX; ++x) {
            const bool r = processTile(x);
            if(!r) break;
            mMaxX = x;
        }
        if(minRectX && maxRectX) {
            mRect = eRectF{*minRectX, y0 - thick,
                           *maxRectX - *minRectX, 2.f * thick};
        }
    }

    int minX() const { return mMinX; }
    int maxX() const { return mMaxX; }

    const eRectF& rect() const {
        return mRect;
    }
private:
    const eMap& mMap;
    int mMinX = 0;
    int mMaxX = 0;
    eRectF mRect{0.f, 0.f, 0.f, 0.f};
};

class eEmptyTiles {
public:
    enum class eType {
        walkObstacle,
        missileObstacle
    };

    eEmptyTiles(const eType type,
                const eMap& map) :
        mType(type), mMap(map) {}

    void initialize(int minX, int maxX,
                    int minY, int maxY,
                    const int x0, const int y0) {
        mMinX = x0;
        mMaxX = x0;
        mMinY = y0;
        mMaxY = y0;

        if(!matches(x0, y0)) return;

        for(int x = x0 - 1; x >= minX; --x) {
            if(!matches(x, y0)) break;
            mMinX = x;
        }

        for(int x = x0 + 1; x <= maxX; ++x) {
            if(!matches(x, y0)) break;
            mMaxX = x;
        }

        for(int y = y0 - 1; y >= minY; --y) {
            bool ok = true;
            for(int x = mMinX; x <= mMaxX; ++x) {
                if(!matches(x, y)) {
                    ok = false;
                    break;
                }
            }
            if(!ok) break;
            mMinY = y;
        }

        for(int y = y0 + 1; y <= maxY; ++y) {
            bool ok = true;
            for(int x = mMinX; x <= mMaxX; ++x) {
                if(!matches(x, y)) {
                    ok = false;
                    break;
                }
            }
            if(!ok) break;
            mMaxY = y;
        }

        mRect = eRectF(float(mMinX), float(mMinY),
                       float(mMaxX - mMinX + 1),
                       float(mMaxY - mMinY + 1));
    }

    int minX() const { return mMinX; }
    int maxX() const { return mMaxX; }
    int minY() const { return mMinY; }
    int maxY() const { return mMaxY; }

    const eRectF& rect() const {
        return mRect;
    }
private:
    bool matches(const int x, const int y) const {
        if(!mMap.inside(x, y)) return false;
        const auto& tile = mMap.tile(x, y);
        switch(mType) {
        case eType::walkObstacle:
            return !tile.walkable();
        case eType::missileObstacle:
            return tile.obstacle();
        }
        return false;
    }

    const eType mType;
    const eMap& mMap;
    eRectF mRect{0.f, 0.f, 0.f, 0.f};
    int mMinX = 0;
    int mMaxX = 0;
    int mMinY = 0;
    int mMaxY = 0;
};

void eMap::generateTiles(const int w, const int h) {
    ::generateTiles(w, h, mTiles);

    mWidth = w;
    mHeight = h;

    mObjectsMap.resize(h, std::vector<std::vector<int>>(w));

    if(!mAllPresent) {
        const int pw = (w + eMapPortion::sBaseDim - 1)/eMapPortion::sBaseDim;
        const int ph = (h + eMapPortion::sBaseDim - 1)/eMapPortion::sBaseDim;
        mPresent.resize(ph, std::vector<bool>(pw, false));
    }

    const auto walkable = [this](const ePointF& from, const ePointF& to) {
        return eMap::walkable(from, to);
    };
    mPathFinderMap.initialize(w, h, walkable);

    const auto filler = [&](std::vector<eRectF>& walkObstacles,
                            std::vector<eRectF>& missileObstacles,
                            const int sx, const int sy) {
        const int dim = eObstaclesMap::sTileSize;
        const int minX = sx*dim - 1;
        const int minY = sy*dim - 1;
        const int maxX = minX + dim + 2;
        const int maxY = minY + dim + 2;

        const bool p1 = hasPortion(minX, minY);
        if(!p1) return false;
        const bool p2 = hasPortion(maxX, maxY);
        if(!p2) return false;

        std::unordered_set<uint64_t> wallsTL;
        std::unordered_set<uint64_t> wallsTR;
        std::unordered_set<uint64_t> processedWalkObstacle;
        std::unordered_set<uint64_t> processedMissileObstacle;

        for(int x = minX; x < maxX; x++) {
            for(int y = minY; y < maxY; y++) {
                const bool r = eMap::inside(x, y);
                if(!r) continue;
                const auto& tile = eMap::tile(x, y);

                const uint64_t key = (uint64_t(uint32_t(x)) << 32) |
                                      uint32_t(y);

                const bool tileWalkable = tile.walkable();

                if(tileWalkable) {
                    const auto& objIds = eMap::objects(x, y);
                    for(const int id : objIds) {
                        const auto& o = *eMap::object(id);
                        const auto type = o.fObjectType;
                        const auto& info = eObjectsInfo::sObjects.get(type);
                        if(info.fWalkable) continue;
                        const auto& pos = o.fPos;
                        const float size = o.fSize;
                        walkObstacles.emplace_back(
                            pos.fX, pos.fY, size, size);
                    }
                } else if(processedWalkObstacle.count(key) == 0) {
                    eEmptyTiles et(eEmptyTiles::eType::walkObstacle, *this);
                    et.initialize(minX, maxX, minY, maxY, x, y);
                    walkObstacles.emplace_back(et.rect());
                    for(int yy = et.minY(); yy <= et.maxY(); ++yy) {
                        for(int xx = et.minX(); xx <= et.maxX(); ++xx) {
                            const uint64_t key = (uint64_t(uint32_t(xx)) << 32) |
                                                 uint32_t(yy);
                            processedWalkObstacle.emplace(key);
                        }
                    }
                }

                const bool missileObstacle = tile.obstacle();

                if(!missileObstacle) {
                    const auto& objIds = eMap::objects(x, y);
                    for(const int id : objIds) {
                        const auto& o = *eMap::object(id);
                        const auto type = o.fObjectType;
                        const auto& info = eObjectsInfo::sObjects.get(type);
                        if(!info.fObstacle) continue;
                        const auto& pos = o.fPos;
                        const float size = o.fSize;
                        missileObstacles.emplace_back(
                            pos.fX, pos.fY, size, size);
                    }
                } else if(processedMissileObstacle.count(key) == 0) {
                    eEmptyTiles et(eEmptyTiles::eType::missileObstacle, *this);
                    et.initialize(minX, maxX, minY, maxY, x, y);
                    missileObstacles.emplace_back(et.rect());
                    for(int yy = et.minY(); yy <= et.maxY(); ++yy) {
                        for(int xx = et.minX(); xx <= et.maxX(); ++xx) {
                            const uint64_t key = (uint64_t(uint32_t(xx)) << 32) |
                                                 uint32_t(yy);
                            processedMissileObstacle.emplace(key);
                        }
                    }
                }

                if(tile.fWallTL) {
                    if(wallsTL.count(key) == 0) {
                        eWallTL tl(*this);
                        tl.initialize(minY, maxY, x, y);
                        const auto& r = tl.rect();
                        walkObstacles.emplace_back(r);
                        missileObstacles.emplace_back(r);

                        {
                            const int minY = tl.minY();
                            const int maxY = tl.maxY();
                            for(int y = minY; y <= maxY; y++) {
                                const uint64_t key = (uint64_t(uint32_t(x)) << 32) |
                                                     uint32_t(y);
                                wallsTL.emplace(key);
                            }
                        }
                    }
                }

                if(tile.fWallTR) {
                    if(wallsTR.count(key) == 0) {
                        eWallTR tr(*this);
                        tr.initialize(minX, maxX, x, y);
                        const auto& r = tr.rect();
                        walkObstacles.emplace_back(r);
                        missileObstacles.emplace_back(r);

                        {
                            const int minX = tr.minX();
                            const int maxX = tr.maxX();
                            for(int x = minX; x <= maxX; x++) {
                                const uint64_t key = (uint64_t(uint32_t(x)) << 32) |
                                                     uint32_t(y);
                                wallsTR.emplace(key);
                            }
                        }
                    }
                }
            }
        }
        return true;
    };
    mObstaclesMap.initialize(filler, w, h);
}

std::shared_ptr<eObject> eMap::addObject(
    const ePointF& pos) {
    const auto obj = std::make_shared<eObject>();
    obj->fPos = pos;
    obj->fObjectId = sNextObjectId++;
    addObject(obj);
    return obj;
}

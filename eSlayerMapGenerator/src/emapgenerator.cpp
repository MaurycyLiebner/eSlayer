#include "eSlayerMapGenerator/emapgenerator.h"

#include "edungeongenerator.h"
#include "eopengenerator.h"

#include "eSlayerMapGenerator/emapsettings.h"
#include "eSlayerMapGenerator/emap.h"

#include <eSlayerHelpers/erect.h>
#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/eobjectsinfo.h>
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

struct eConnection {
    int fX;
    int fY;
    int fW;
    int fH;
};

struct eEdge {
    int fX1;
    int fY1;
    int fX2;
    int fY2;
};

std::optional<eEdge> middlePortionInt(const eEdge& e, const int length) {
    const int dx = e.fX2 - e.fX1;
    const int dy = e.fY2 - e.fY1;

    const int edgeLength = std::abs(dx) + std::abs(dy); // axis-aligned
    if(length > edgeLength) return std::nullopt;

           // midpoint (integer)
    const int mx = (e.fX1 + e.fX2) / 2;
    const int my = (e.fY1 + e.fY2) / 2;

    const int half1 = length / 2;        // floor
    const int half2 = length - half1;    // ensures total length is exact

    if(dx == 0) {
        // vertical
        return eEdge{
            mx,
            my - half1,
            mx,
            my + half2
        };
    } else {
        // horizontal
        return eEdge{
            mx - half1,
            my,
            mx + half2,
            my
        };
    }
}

eConnection extrudeEdge(const eEdge& e, const int displacement) {
    eConnection out{};

    const int d = std::abs(displacement);

    if(e.fY1 == e.fY2) {
        // horizontal edge → extrude vertically (both up and down)
        const int x1 = std::min(e.fX1, e.fX2);
        const int x2 = std::max(e.fX1, e.fX2);

        out.fX = x1;
        out.fW = x2 - x1;

        out.fY = e.fY1 - d;
        out.fH = 2 * d;
    } else {
        // vertical edge → extrude horizontally (both left and right)
        const int y1 = std::min(e.fY1, e.fY2);
        const int y2 = std::max(e.fY1, e.fY2);

        out.fY = y1;
        out.fH = y2 - y1;

        out.fX = e.fX1 - d;
        out.fW = 2 * d;
    }

    return out;
}

std::optional<eEdge> sharedEdge(const eRect& A, const eRect& B) {
    // A is left of B
    if(A.fX + A.fW == B.fX) {
        const int y1 = std::max(A.fY, B.fY);
        const int y2 = std::min(A.fY + A.fH, B.fY + B.fH);
        if(y1 < y2) return eEdge{B.fX, y1, B.fX, y2};
    }

    // A is right of B
    if(B.fX + B.fW == A.fX) {
        const int y1 = std::max(A.fY, B.fY);
        const int y2 = std::min(A.fY + A.fH, B.fY + B.fH);
        if(y1 < y2) return eEdge{A.fX, y1, A.fX, y2};
    }

    // A is above B
    if(A.fY + A.fH == B.fY) {
        const int x1 = std::max(A.fX, B.fX);
        const int x2 = std::min(A.fX + A.fW, B.fX + B.fW);
        if(x1 < x2) return eEdge{x1, B.fY, x2, B.fY};
    }

    // A is below B
    if(B.fY + B.fH == A.fY) {
        const int x1 = std::max(A.fX, B.fX);
        const int x2 = std::min(A.fX + A.fW, B.fX + B.fW);
        if(x1 < x2) return eEdge{x1, A.fY, x2, A.fY};
    }

    return std::nullopt; // no shared edge
}

class eDungeon {
public:
    eDungeon() {}
    eDungeon(const std::string& name,
             const int x, const int y,
             const int w, const int h,
             const std::shared_ptr<eMap>& map,
             const eAreaSettings& settings,
             const eMapSettings& mapSettings,
             const int margin) :
        mName(name),
        mX(x), mY(y),
        mWidth(w), mHeight(h),
        mMargin(margin),
        mMap(map),
        mSettings(settings),
        mMapSettings(mapSettings) {}

    void addConnection(const eConnection& conn) {
        mConnecitons.emplace_back(conn);
    }

    void shift(const int dx, const int dy) {
        mX += dx;
        mY += dy;
        for(auto& conn : mConnecitons) {
            conn.fX += dx;
            conn.fY += dy;
        }
    }

    eRect rect() const {
        return eRect{mX, mY, mWidth, mHeight};
    }

    void setExtendedRect(const eRect& rect) {
        mExtendedRect = rect;
    }

    void generate() const {
        const auto rect = eDungeon::rect();
        std::vector<eChamber> chambers;
        std::vector<eRect> doors;
        for(const auto& c : mConnecitons) {
            const eRect connRect{c.fX, c.fY, c.fW, c.fH};
            eRect connIn;
            eRect::intersection(rect, connRect, connIn);
            if(connIn.fW <= 0 || connIn.fH <= 0) continue;
            chambers.emplace_back(connIn);
        }

        bool rectWalls = false;
        bool fillEmptySapces = false;

        switch(mSettings.fType) {
        case eAreaType::dungeon: {
            rectWalls = true;
            fillEmptySapces = false;
            eDungeonGenerator::generate(
                rect, chambers, doors);
        } break;
        case eAreaType::open: {
            rectWalls = false;
            fillEmptySapces = true;
            eOpenGenerator::generate(
                rect, chambers, doors, mMargin);
        } break;
        }

        const auto inRect = [&](const int x, const int y,
                                const eChamber* const skip = nullptr) {
            const ePoint p{x, y};
            for(const auto& c : chambers) {
                if(&c == skip) continue;
                const bool r = c.contains(p);
                if(r) return true;
            }
            return false;
        };

        const auto inDoorsRect = [&](const int x, const int y,
                                     const eWallType type) {
            const ePoint p{x, y};
            for(const auto& rect : doors) {
                switch(type) {
                case eWallType::topLeft: {
                    if(rect.fW != 0) continue;
                    const eRect rect1{rect.fX, rect.fY, 1, rect.fH};
                    const bool r = rect1.contains(p);
                    if(r) return true;
                } break;
                case eWallType::topRight: {
                    if(rect.fH != 0) continue;
                    const eRect rect1{rect.fX, rect.fY, rect.fW, 1};
                    const bool r = rect1.contains(p);
                    if(r) return true;
                } break;
                }
            }
            return false;
        };

        const auto shouldWall = [&](const int x, const int y,
                                    const eChamber& sc,
                                    bool& wallTL, bool& wallTR) {
            wallTL = true;
            wallTR = true;
            const ePoint p{x, y};
            for(const auto& c : chambers) {
                if(&c == &sc) continue;
                const auto inside = [&c, &sc](const int x, const int y) {
                    return c.contains({x, y}) || sc.contains({x, y});
                };
                const bool wallTL_ = inside(x, y) != inside(x - 1, y);
                if(!wallTL_) wallTL = false;
                const bool wallTR_ = inside(x, y) != inside(x, y - 1);
                if(!wallTR_) wallTR = false;
            }
            if(x <= rect.fX || x >= rect.fX + rect.fW - 1) {
                wallTL = false;
            }
            if(y <= rect.fY || y >= rect.fY + rect.fH - 1) {
                wallTR = false;
            }
        };

        const auto terrType = mSettings.fTerrainType;
        const auto& terrTypeInfo = eTerrsTexturesData::get(terrType);
        const auto floorUse = terrTypeInfo.fFloorUse;
        const auto& floor = terrTypeInfo.fFloor;
        auto& tiles = mMap->mTiles;

        const auto setTileTerrain = [&](const int x, const int y,
                                        eTile& dst) {
            dst.fTerrainType = terrType;
            const int nTypes = floor.size();
            switch(floorUse) {
            case eFloorUse::random: {
                dst.fTileType = eRand::rand(1, nTypes);
            } break;
            case eFloorUse::tiled: {
                const int dim = sqrt(nTypes);
                dst.fTileType = 1 + (x % dim) + (y % dim) * dim;
            } break;
            }
        };

        const auto tryAddObject =
            [this](const eChamber& c,
                   const int x, const int y,
                   const int objMargin,
                   const std::vector<eTypeProbability>& objs) {
            if(objs.empty()) return;
            for(int dx = -objMargin; dx <= objMargin; dx++) {
                const int xx = x + dx;
                for(int dy = -objMargin; dy <= objMargin; dy++) {
                    const int yy = y + dy;
                    if(!c.contains({xx, yy})) return;
                    const auto& objs = mMap->objects(x + dx, y + dy);
                    if(!objs.empty()) return;
                }
            }

            const int startId = eRand::rand() % objs.size();
            for(int i = 0; i < objs.size(); i++) {
                const int idx = (startId + i) % objs.size();
                const auto& o = objs[idx];
                const bool add = eRand::randChance(o.fProbability);
                if(!add) continue;
                const auto& info = eObjectsInfo::sObjects.get(o.fType);
                auto& obj = *mMap->addObject();
                obj.fObjectType = o.fType;
                obj.fSubtype = eRand::rand();
                obj.fPos.fX = x;
                obj.fPos.fY = y;
                obj.fSize = info.fSize;
                return;
            }
        };

        const auto& ms = mSettings.fMonsters;
        auto& mareas = mMap->mMonsterAreas;
        for(const auto& sc : chambers) {
            for(const auto& rect : sc.fRects) {
                auto& marea = mareas.emplace_back();
                marea.fRect = rect;
                marea.fSettings = ms;

                const int minX = rect.fX;
                const int maxX = rect.fX + rect.fW - 1;
                const int minY = rect.fY;
                const int maxY = rect.fY + rect.fH - 1;
                for(int x = minX; x <= maxX + 1; x++) {
                    for(int y = minY; y <= maxY + 1; y++) {
                        auto& dst = tiles[y][x];
                        if(x <= maxX && y <= maxY) {
                            setTileTerrain(x, y, dst);
                        }

                        const int m = mSettings.fObjectsMargin;
                        const auto& objs = mSettings.fObjects;
                        tryAddObject(sc, x, y, m, objs);

                        bool wallTL = sc.wallTL({x, y});
                        bool wallTR = sc.wallTR({x, y});
                        if(!rectWalls) {
                            shouldWall(x, y, sc, wallTL, wallTR);
                        }

                        if(wallTL && !dst.fWallTL) {
                            const bool doors = inDoorsRect(x, y, eWallType::topLeft);
                            const bool tl = x == minX && y != maxY + 1;
                            const bool br = x == maxX + 1 && y != maxY + 1;
                            if(tl || br) {
                                dst.fTerrainType = terrType;
                                dst.fWallTL = eTile::encodeWall(true, doors, false, br, 0);
                            }
                        }
                        if(wallTR && !dst.fWallTR) {
                            const bool doors = inDoorsRect(x, y, eWallType::topRight);
                            const bool tr = y == minY && x != maxX + 1;
                            const bool bl = y == maxY + 1 && x != maxX + 1;
                            if(tr || bl) {
                                dst.fTerrainType = terrType;
                                dst.fWallTR = eTile::encodeWall(true, doors, false, bl, 0);
                            }
                        }
                    }
                }
            }
        }

        if(fillEmptySapces) {
            const auto treeId = eObjectsInfo::sObjects.id("tree");
            const auto& treeInfo = eObjectsInfo::sObjects.get(treeId);
            const auto& rect = mExtendedRect;
            for(int x = rect.fX; x < rect.fX + rect.fW; x++) {
                for(int y = rect.fY; y < rect.fY + rect.fH; y++) {
                    const bool r = inRect(x, y, nullptr);
                    if(r) continue;
                    auto& dst = tiles[y][x];
                    setTileTerrain(x, y, dst);
                    {
                        const bool r = inRect(x + 1, y, nullptr);
                        if(r) continue;
                    }
                    {
                        const bool r = inRect(x, y + 1, nullptr);
                        if(r) continue;
                    }

                    const int m = mSettings.fOutsideObjectsMargin;
                    const auto& objs = mSettings.fOutsideObjects;
                    tryAddObject({{rect}}, x, y, m, objs);
                }
            }
        }

        mMap->fillPathFinderMap();
    }

    void generateWalls() const {
        const auto rect = eDungeon::rect();
        const auto terrType = mSettings.fTerrainType;
        const auto& terrTypeInfo = eTerrsTexturesData::get(terrType);

        const auto& tlWalls = terrTypeInfo.fTLWalls;
        const auto& trWalls = terrTypeInfo.fTRWalls;
        const auto& tlDoors = terrTypeInfo.fTLDoors;
        const auto& trDoors = terrTypeInfo.fTRDoors;
        const auto& tlStairsDown = terrTypeInfo.fTLStairsDown;
        const auto& trStairsDown = terrTypeInfo.fTRStairsDown;
        const auto& tlStairsUp = terrTypeInfo.fTLStairsUp;
        const auto& trStairsUp = terrTypeInfo.fTRStairsUp;

        const auto& blWalls = terrTypeInfo.fBLWalls;
        const auto& brWalls = terrTypeInfo.fBRWalls;
        const auto& blDoors = terrTypeInfo.fBLDoors;
        const auto& brDoors = terrTypeInfo.fBRDoors;
        const auto& blStairsDown = terrTypeInfo.fBLStairsDown;
        const auto& brStairsDown = terrTypeInfo.fBRStairsDown;
        const auto& blStairsUp = terrTypeInfo.fBLStairsUp;
        const auto& brStairsUp = terrTypeInfo.fBRStairsUp;

        auto& tiles = mMap->mTiles;
        const int h = mMap->height();
        const int w = mMap->width();
        const auto hasTLWall = [w, h, &tiles](const int x, const int y,
                                              const bool doors,
                                              const bool other) {
            if(x < 0 || y < 0 || x >= w || y >= h) return false;
            const auto& src = tiles[y][x];
            if(!src.fWallTL) return false;
            const bool d = eTile::doors(src.fWallTL);
            const bool o = eTile::other(src.fWallTL);
            return d == doors && o == other;
        };

        const auto has0TLWall = [&tiles](const int x, const int y) {
            const auto& src = tiles[y][x];
            if(!src.fWallTL) return false;
            const uint8_t type = eTile::type(src.fWallTL);
            return type == 0;
        };

        const auto hasTLDoors = [&tiles](const int x, const int y) {
            const auto& src = tiles[y][x];
            return eTile::doors(src.fWallTL);
        };

        const auto hasTLOther = [&tiles](const int x, const int y) {
            const auto& src = tiles[y][x];
            return eTile::other(src.fWallTL);
        };

        const auto& tlWallsSizes = tlWalls.fSizes;
        const int maxTLSizeClamp = tlWallsSizes.empty() ? 0 :
            tlWallsSizes.back();
        const auto& tlDoorsSizes = tlDoors.fSizes;
        const int maxTLDoorsSizeClamp = tlDoorsSizes.empty() ? 0 :
            tlDoorsSizes.back();

        const auto& brWallsSizes = brWalls.fSizes;
        const int maxBRSizeClamp = brWallsSizes.empty() ? 0 :
            brWallsSizes.back();
        const auto& brDoorsSizes = brDoors.fSizes;
        const int maxBRDoorsSizeClamp = brDoorsSizes.empty() ? 0 :
            brDoorsSizes.back();

        const auto maxTLSizeBase = [&](const int x, const int y,
                                       const int maxSize,
                                       const bool doors,
                                       const bool other) {
            for(int i = 0; i < maxSize; i++) {
                const bool r = hasTLWall(x, y + i, doors, other);
                if(!r) return i;
            }
            return maxSize;
        };
        const auto maxTLSize = [&](const int x, const int y,
                                   const bool doors,
                                   const bool other) {
            const int maxSize = other ?
                (doors ? maxBRDoorsSizeClamp : maxBRSizeClamp) :
                (doors ? maxTLDoorsSizeClamp : maxTLSizeClamp);
            return maxTLSizeBase(x, y, maxSize, doors, other);
        };

        const std::vector<int> ref;

        const auto chooseVec = [&ref](const int maxSize,
                                      const std::vector<std::vector<int>>& d)
            -> const std::vector<int>& {
            for(int i = d.size() - 1; i >= 0; i--) {
                const auto& v = d[i];
                if(v.size() <= maxSize) return v;
            }
            return ref;
        };

        const auto chooseTLVec = [&](const int maxSize,
                                     const bool doors,
                                     const bool other)
            -> const std::vector<int>& {
            const auto& d = other ?
                (doors ? brDoors.fDataIds : brWalls.fDataIds) :
                (doors ? tlDoors.fDataIds : tlWalls.fDataIds);
            return chooseVec(maxSize, d);
        };

        const auto chooseTLStairsDownVec = [&](const int maxSize,
                                               const bool other)
            -> const std::vector<int>& {
            const auto& d = other ? brStairsDown.fDataIds :
                                    tlStairsDown.fDataIds;
            return chooseVec(maxSize, d);
        };

        const auto chooseTLStairsUpVec = [&](const int maxSize,
                                             const bool other)
            -> const std::vector<int>& {
            const auto& d = other ? brStairsUp.fDataIds :
                                    tlStairsUp.fDataIds;
            return chooseVec(maxSize, d);
        };

        const auto hasTRWall = [w, h, &tiles](const int x, const int y,
                                              const bool doors,
                                              const bool other) {
            if(x < 0 || y < 0 || x >= w || y >= h) return false;
            const auto& src = tiles[y][x];
            if(!src.fWallTR) return false;
            const bool d = eTile::doors(src.fWallTR);
            const bool o = eTile::other(src.fWallTR);
            return d == doors && o == other;
        };

        const auto has0TRWall = [&tiles](const int x, const int y) {
            const auto& src = tiles[y][x];
            if(!src.fWallTR) return false;
            const uint8_t type = eTile::type(src.fWallTR);
            return type == 0;
        };

        const auto hasTRDoors = [&tiles](const int x, const int y) {
            const auto& src = tiles[y][x];
            return eTile::doors(src.fWallTR);
        };

        const auto hasTROther = [&tiles](const int x, const int y) {
            const auto& src = tiles[y][x];
            return eTile::other(src.fWallTR);
        };

        const auto& trWallsSizes = trWalls.fSizes;
        const int maxTRSizeClamp = trWallsSizes.empty() ? 0 :
            trWallsSizes.back();
        const auto& trDoorsSizes = trDoors.fSizes;
        const int maxTRDoorsSizeClamp = trDoorsSizes.empty() ? 0 :
            trDoorsSizes.back();

        const auto& blWallsSizes = blWalls.fSizes;
        const int maxBLSizeClamp = blWallsSizes.empty() ? 0 :
            blWallsSizes.back();
        const auto& blDoorsSizes = blDoors.fSizes;
        const int maxBLDoorsSizeClamp = blDoorsSizes.empty() ? 0 :
            blDoorsSizes.back();

        const auto maxTRSizeBase = [&](const int x, const int y,
                                       const int maxSize,
                                       const bool doors,
                                       const bool other) {
            for(int i = 0; i < maxSize; i++) {
                const bool r = hasTRWall(x + i, y, doors, other);
                if(!r) return i;
            }
            return maxSize;
        };

        const auto maxTRSize = [&](const int x, const int y,
                                   const bool doors,
                                   const bool other) {
            const int maxSize = other ?
                (doors ? maxBLDoorsSizeClamp : maxBLSizeClamp) :
                (doors ? maxTRDoorsSizeClamp : maxTRSizeClamp);
            return maxTRSizeBase(x, y, maxSize, doors, other);
        };

        const auto chooseTRVec = [&](const int maxSize,
                                     const bool doors,
                                     const bool other)
            -> const std::vector<int>& {
            const auto& d = other ?
                (doors ? blDoors.fDataIds : blWalls.fDataIds) :
                (doors ? trDoors.fDataIds : trWalls.fDataIds);
            return chooseVec(maxSize, d);
        };

        const auto& trStairDownSizes = trStairsDown.fSizes;
        const int maxTRStairsDownClamp = trStairDownSizes.empty() ? 0 :
            trStairDownSizes.back();
        const auto& trStairUpSizes = trStairsUp.fSizes;
        const int maxTRStairsUpClamp = trStairUpSizes.empty() ? 0 :
            trStairUpSizes.back();

        const auto& blStairDownSizes = blStairsDown.fSizes;
        const int maxBLStairsDownClamp = blStairDownSizes.empty() ? 0 :
            blStairDownSizes.back();
        const auto& blStairUpSizes = blStairsUp.fSizes;
        const int maxBLStairsUpClamp = blStairUpSizes.empty() ? 0 :
            blStairUpSizes.back();

        const auto maxTRStairsDownSize = [&](const int x, const int y,
                                             const bool other) {
            const int maxSize = other ? maxBLStairsDownClamp :
                                        maxTRStairsDownClamp;
            return maxTRSizeBase(x, y, maxSize, false, other);
        };

        const auto maxTRStairsUpSize = [&](const int x, const int y,
                                           const bool other) {
            const int maxSize = other ? maxBLStairsUpClamp :
                                        maxTRStairsUpClamp;
            return maxTRSizeBase(x, y, maxSize, false, other);
        };

        const auto& tlStairDownSizes = tlStairsDown.fSizes;
        const int maxTLStairsDownClamp = tlStairDownSizes.empty() ? 0 :
            tlStairDownSizes.back();
        const auto& tlStairUpSizes = tlStairsUp.fSizes;
        const int maxTLStairsUpClamp = tlStairUpSizes.empty() ? 0 :
            tlStairUpSizes.back();

        const auto& brStairDownSizes = brStairsDown.fSizes;
        const int maxBRStairsDownClamp = brStairDownSizes.empty() ? 0 :
            brStairDownSizes.back();
        const auto& brStairUpSizes = brStairsUp.fSizes;
        const int maxBRStairsUpClamp = brStairUpSizes.empty() ? 0 :
            brStairUpSizes.back();

        const auto maxTLStairsDownSize = [&](const int x, const int y,
                                             const bool other) {
            const int maxSize = other ? maxBRStairsDownClamp :
                                        maxTLStairsDownClamp;
            return maxTLSizeBase(x, y, maxSize, false, other);
        };

        const auto maxTLStairsUpSize = [&](const int x, const int y,
                                           const bool other) {
            const int maxSize = other ? maxBRStairsUpClamp :
                                        maxTLStairsUpClamp;
            return maxTLSizeBase(x, y, maxSize, false, other);
        };

        const auto chooseTRStairsDownVec = [&](const int maxSize,
                                               const bool other)
            -> const std::vector<int>& {
            const auto& d = other ? blStairsUp.fDataIds :
                                    trStairsDown.fDataIds;
            return chooseVec(maxSize, d);
        };

        const auto chooseTRStairsUpVec = [&](const int maxSize,
                                             const bool other)
            -> const std::vector<int>& {
            const auto& d = other ? blStairsUp.fDataIds :
                                    trStairsUp.fDataIds;
            return chooseVec(maxSize, d);
        };

        struct eExtendedStairs : public eMapStairs {
            const std::vector<int>* fVPtr = nullptr;
        };

        std::vector<eExtendedStairs> stairsOptions;

        const int stairsMargin = 3;
        const auto spaceForStairs = [&](
            const int x, const int y,
            const eWallType type) {
            const int idx = type == eWallType::topLeft ? 1 : 0;
            const int idy = type == eWallType::topRight ? 1 : 0;
            const int jdx = type == eWallType::topRight ? 1 : 0;
            const int jdy = type == eWallType::topLeft ? 1 : 0;
            for(int i = 1; i <= stairsMargin; i++) {
                const int jMargin = stairsMargin/2 + 1;
                for(int j = -jMargin; j <= jMargin; j++) {
                    const int xx = x + i*idx + j*jdx;
                    const int yy = y + i*idy + j*jdy;
                    const bool i = mMap->inside(xx, yy);
                    if(!i) return false;
                    const auto& tile = mMap->tile(xx, yy);
                    if(tile.fWallTL) return false;
                    if(tile.fWallTR) return false;
                    const bool w = tile.walkable();
                    if(!w) return false;
                }
            }
            return true;
        };

        for(int x = rect.fX; x < rect.fX + rect.fW; x++) {
            for(int y = rect.fY; y < rect.fY + rect.fH; y++) {
                {
                    const bool r = has0TLWall(x, y);
                    if(r) {
                        const bool doors = hasTLDoors(x, y);
                        const bool other = hasTLOther(x, y);
                        const int maxSize = maxTLSize(x, y, doors, other);
                        const auto& v = chooseTLVec(maxSize, doors, other);
                        const int size = v.size();
                        for(int dy = 0; dy < size; dy++) {
                            auto& dst = tiles[y + dy][x];
                            dst.fWallTL = eTile::encodeWall(true, doors, false, other, v[dy]);
                        }
                    }
                }
                {
                    const bool r = has0TRWall(x, y);
                    if(r) {
                        const bool doors = hasTRDoors(x, y);
                        const bool other = hasTROther(x, y);
                        const int maxSize = maxTRSize(x, y, doors, other);
                        const auto& v = chooseTRVec(maxSize, doors, other);
                        const int size = v.size();
                        for(int dx = 0; dx < size; dx++) {
                            auto& dst = tiles[y][x + dx];
                            dst.fWallTR = eTile::encodeWall(true, doors, false, other, v[dx]);
                        }
                    }
                }

                const auto& tile = tiles[y][x];
                if(tile.fWallTL) {
                    const bool doors = eTile::doors(tile.fWallTL);
                    const bool other = eTile::other(tile.fWallTL);
                    const bool space = spaceForStairs(x, y, eWallType::topLeft);
                    if(!doors && space) {
                        {
                            const int maxSize = maxTLStairsDownSize(x, y, other);
                            const auto& v = chooseTLStairsDownVec(maxSize, other);
                            const int size = v.size();
                            if(size > 0) {
                                auto& s = stairsOptions.emplace_back();
                                s.fX = x;
                                s.fY = y;
                                s.fDim = size;
                                s.fWallType = eWallType::topLeft;
                                s.fStairsDir = eConnectionDir::down;
                                s.fVPtr = &v;
                            }
                        }
                        {
                            const int maxSize = maxTLStairsUpSize(x, y, other);
                            const auto& v = chooseTLStairsUpVec(maxSize, other);
                            const int size = v.size();
                            if(size > 0) {
                                auto& s = stairsOptions.emplace_back();
                                s.fX = x;
                                s.fY = y;
                                s.fDim = size;
                                s.fWallType = eWallType::topLeft;
                                s.fStairsDir = eConnectionDir::up;
                                s.fVPtr = &v;
                            }
                        }
                    }
                }
                if(tile.fWallTR) {
                    const bool doors = eTile::doors(tile.fWallTR);
                    const bool other = eTile::other(tile.fWallTR);
                    const bool space = spaceForStairs(x, y, eWallType::topRight);
                    if(!doors && space) {
                        {
                            const int maxSize = maxTRStairsDownSize(x, y, other);
                            const auto& v = chooseTRStairsDownVec(maxSize, other);
                            const int size = v.size();
                            if(size > 0) {
                                auto& s = stairsOptions.emplace_back();
                                s.fX = x;
                                s.fY = y;
                                s.fDim = size;
                                s.fWallType = eWallType::topRight;
                                s.fStairsDir = eConnectionDir::down;
                                s.fVPtr = &v;
                            }
                        }
                        {
                            const int maxSize = maxTRStairsUpSize(x, y, other);
                            const auto& v = chooseTRStairsUpVec(maxSize, other);
                            const int size = v.size();
                            if(size > 0) {
                                auto& s = stairsOptions.emplace_back();
                                s.fX = x;
                                s.fY = y;
                                s.fDim = size;
                                s.fWallType = eWallType::topRight;
                                s.fStairsDir = eConnectionDir::up;
                                s.fVPtr = &v;
                            }
                        }
                    }
                }
            }
        }

        const int stairsMinDist = 5;
        eRand::randomShuffle(stairsOptions);
        std::vector<eExtendedStairs> used;
        for(const auto& it : mSettings.fConnections) {
            const auto& conn = it.second;
            const auto type = conn.fType;
            if(type != eConnectionType::stairs) continue;
            const auto& name = it.first;
            const int mapId = eMapsSettings::sMaps.id(name);
            if(mapId < 0) continue;
            if(stairsOptions.empty()) continue;
            eExtendedStairs stairs;

            bool tooClose = false;
            do {
                if(stairsOptions.empty()) break;
                stairs = stairsOptions.back();
                stairsOptions.pop_back();
                for(const auto& u : used) {
                    const float dist = ePointF::distance(
                        {stairs.fX, stairs.fY}, {u.fX, u.fY});
                    if(dist < stairsMinDist) {
                        tooClose = true;
                        break;
                    }
                }
            } while(tooClose);
            if(tooClose) continue;
            const int x = stairs.fX;
            const int y = stairs.fY;
            used.emplace_back(stairs);
            const auto& v = *stairs.fVPtr;
            const int size = v.size();
            switch(stairs.fWallType) {
            case eWallType::topLeft: {
                for(int dy = 0; dy < size; dy++) {
                    auto& dst = tiles[y + dy][x];
                    mMap->addStairs(x, y, stairs.fWallType,
                                    conn.fDir, v[dy], mapId);
                }
            } break;
            case eWallType::topRight: {
                for(int dx = 0; dx < size; dx++) {
                    auto& dst = tiles[y][x + dx];
                    mMap->addStairs(x, y, stairs.fWallType,
                                    conn.fDir, v[dx], mapId);
                }
            } break;
            }
        }
    }

    const std::string& name() const {
        return mName;
    }
private:
    std::string mName;

    eRect mExtendedRect;

    int mX;
    int mY;
    int mWidth;
    int mHeight;

    int mMargin;

    std::vector<eConnection> mConnecitons;

    std::shared_ptr<eMap> mMap;
    eAreaSettings mSettings;
    eMapSettings mMapSettings;
};

enum class eDir {
    topLeft, bottomRight,
    topRight, bottomLeft
};

struct eAreaPlace {
    int fX;
    int fY;

    bool operator<(const eAreaPlace& other) const {
        if(fX != other.fX) return fX < other.fX;
        return fY < other.fY;
    }

    eAreaPlace shifted(const int dx, const int dy) const {
        return {fX + dx, fY + dy};
    }
};

class eAreaPlacer {
public:
    eAreaPlacer(const int areaDim) :
        mAreaDim(areaDim) {
        mUsedAreas.resize(mMargin, std::vector<bool>(mMargin, false));
    }

    eAreaPlace iniPlace() {
        const int xy = mMargin/2;
        return eAreaPlace{xy, xy};
    }

    ePoint pos(const eAreaPlace& place) const {
        return {place.fX*mAreaDim,
                place.fY*mAreaDim};
    }

    eRect boundingRect(const eAreaPlace& place) const {
        return {place.fX*mAreaDim,
                place.fY*mAreaDim,
                mAreaDim, mAreaDim};
    }

    const eRect& boundingRect() const {
        return mBoundingRect;
    }

    bool areaUsed(const eAreaPlace& area) const {
        return mUsedAreas[area.fY][area.fX];
    }

    eAreaPlace choosePlace(const eAreaPlace& from) {
        std::vector<eDir> options {
            eDir::topLeft, eDir::topRight, eDir::bottomLeft, eDir::bottomRight
        };
        eRand::randomShuffle(options);

        int usedX;
        int usedY;
        for(const auto d : options) {
            switch(d) {
            case eDir::topLeft:
                usedX = from.fX - 1;
                usedY = from.fY;
                break;
            case eDir::bottomRight:
                usedX = from.fX + 1;
                usedY = from.fY;
                break;
            case eDir::topRight:
                usedX = from.fX;
                usedY = from.fY - 1;
                break;
            case eDir::bottomLeft:
                usedX = from.fX;
                usedY = from.fY + 1;
                break;
            }

            if(!mUsedAreas[usedY][usedX]) {
                mUsedAreas[usedY][usedX] = true;
                const auto result = eAreaPlace{usedX, usedY};
                const auto pos = eAreaPlacer::pos(result);
                const eRect rect{pos.fX, pos.fY, mAreaDim, mAreaDim};
                if(mBoundingRect.fW == 0) {
                    mBoundingRect = rect;
                } else {
                    mBoundingRect.sum(rect);
                }
                return result;
            }
        }
        return from;
    }

    eConnection chooseConnection(const eAreaPlace& from,
                                 const eAreaPlace& to,
                                 const int width, const int halfLen) {
        const auto fromRect = boundingRect(from);
        const auto toRect = boundingRect(to);
        const auto edgeO = sharedEdge(fromRect, toRect);
        if(edgeO == std::nullopt) return eConnection{0, 0, 0, 0};
        const auto& edge = edgeO.value();
        const auto connEdgeO = middlePortionInt(edge, width);
        if(connEdgeO == std::nullopt) return eConnection{0, 0, 0, 0};
        const auto& connEdge = connEdgeO.value();
        return extrudeEdge(connEdge, halfLen);
    }
private:
    const int mAreaDim = 80;
    const int mMargin = 20;
    std::vector<std::vector<bool>> mUsedAreas;
    eRect mBoundingRect{0, 0, 0, 0};
};

std::shared_ptr<eMap>
eMapGenerator::generate(const std::string& name) const {
    const auto mapId = eMapsSettings::sMaps.id(name);
    if(mapId < 0) {
        eRuntimeThrow("No map \"" + name + "\" settings found.");
    }
    const auto& mapSettings = eMapsSettings::sMaps.get(mapId);
    if(mapSettings.fAreas.size() == 0) {
        eRuntimeThrow("No areas to generate for \"" + name + "\"");
    }
    const auto result = std::make_shared<eMap>(name);
    result->mAllPresent = true;

    const int areaDim = 80;

    std::map<eAreaPlace, eDungeon> areas;

    eAreaPlacer placer(areaDim);
    const auto firstPlace = placer.iniPlace();

    std::function<eAreaPlace(const std::string& name,
                             const eAreaSettings& settings,
                             const eAreaPlace& nextTo)> genArea;
    const int connWidth = 4;
    const int connHalfLen = 4;
    auto& terrTypes = result->mTerrainTypes;
    auto& objTypes = result->mObjectTypes;
    genArea = [&](const std::string& name,
                  const eAreaSettings& settings,
                  const eAreaPlace& nextTo) {
        const auto terrType = settings.fTerrainType;
        terrTypes.emplace(terrType);

        for(const auto& o : settings.fObjects) {
            objTypes.emplace(o.fType);
        }

        const auto place = placer.choosePlace(nextTo);
        const auto pos = placer.pos(place);

        const int x = pos.fX;
        const int y = pos.fY;

        auto& area = areas[place];
        area = eDungeon(name, x, y, areaDim, areaDim,
                        result, settings, mapSettings,
                        connHalfLen);

        for(const auto& it : settings.fConnections) {
            const auto& conn = it.second;
            const auto connType = conn.fType;
            if(connType != eConnectionType::plain) continue;
            const auto name = it.first;
            const int settingsId = mapSettings.fAreas.id(name);
            const auto settings = mapSettings.fAreas.get(settingsId);
            const auto connPlace = genArea(name, settings, place);
            const auto conn_ = placer.chooseConnection(
                place, connPlace, connWidth, connHalfLen);
            area.addConnection(conn_);
            auto& connArea = areas[connPlace];
            connArea.addConnection(conn_);
        }

        return place;
    };

    const auto name0 = mapSettings.fAreas.name(0);
    const eAreaSettings& settings = mapSettings.fAreas.get(0);
    genArea(name0, settings, firstPlace);

    const int extMargin = 10;
    auto rect = placer.boundingRect();
    for(auto& it : areas) {
        const auto& place = it.first;
        auto& area = it.second;
        area.shift(extMargin - rect.fX, extMargin - rect.fY);

        eRect extRect = area.rect();
        if(!placer.areaUsed(place.shifted(1, 0))) {
            extRect.fW += extMargin;
        }
        if(!placer.areaUsed(place.shifted(0, 1))) {
            extRect.fH += extMargin;
        }
        if(!placer.areaUsed(place.shifted(-1, 0))) {
            extRect.fX -= extMargin;
            extRect.fW += extMargin;
        }
        if(!placer.areaUsed(place.shifted(0, -1))) {
            extRect.fY -= extMargin;
            extRect.fH += extMargin;
        }
        area.setExtendedRect(extRect);
    }
    rect.fX = extMargin;
    rect.fY = extMargin;

    result->generateTiles(rect.fW + 2*extMargin + 1,
                          rect.fH + 2*extMargin + 1);
    bool first = true;
    for(const auto& it : areas) {
        const auto& area = it.second;
        const auto& name = area.name();
        const int id = mapSettings.fAreas.id(name);
        if(id < 0) continue;
        eMapArea mapArea;
        mapArea.fMapId = mapId;
        mapArea.fAreaId = id;
        const auto rect = area.rect();
        mapArea.fRect = rect;
        result->mAreas.add(name, mapArea);
        area.generate();
        if(first) {
            first = false;
            bool found = false;
            for(int dist = 0; dist < 100; dist++) {
                for(int x = dist; x >= -dist; x--) {
                    for(int y = dist; y >= -dist; y--) {
                        if(std::abs(x) != dist && std::abs(y) != dist) continue;
                        const ePoint pos{rect.fX + rect.fW/2 + x,
                                         rect.fY + rect.fH/2 + y};
                        const bool w = result->walkable(pos);
                        if(w) {
                            result->mSpawnPos = pos;
                            found = true;
                            break;
                        }
                    }
                    if(found) break;
                }
                if(found) break;
            }
        }
    }
    for(const auto& it : areas) {
        const auto& area = it.second;
        area.generateWalls();
    }

    result->updateObjectsMap();

    const auto& ms = settings.fMonsters;
    const auto& types = ms.fTypes;
    for(const auto& type : types) {
        result->mUnitTypes.emplace(type.fType);
    }

    return result;
}

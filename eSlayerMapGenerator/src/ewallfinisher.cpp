#include "ewallfinisher.h"

#include <eSlayerHelpers/eterrstexturesdata.h>

void eWallFinisher::finish(
    const eRect& rect,
    const eAreaSettings& settings,
    eMap& map) {
    const auto terrType = settings.fTerrainType;
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

    auto& tiles = map.mTiles;
    const int h = map.height();
    const int w = map.width();
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
                const bool i = map.inside(xx, yy);
                if(!i) return false;
                const auto& tile = map.tile(xx, yy);
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
    for(const auto& it : settings.fConnections) {
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
                map.addStairs(x, y, stairs.fWallType,
                                conn.fDir, v[dy], mapId);
            }
        } break;
        case eWallType::topRight: {
            for(int dx = 0; dx < size; dx++) {
                auto& dst = tiles[y][x + dx];
                map.addStairs(x, y, stairs.fWallType,
                                conn.fDir, v[dx], mapId);
            }
        } break;
        }
    }
}

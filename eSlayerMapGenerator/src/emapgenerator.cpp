#include "eSlayerMapGenerator/emapgenerator.h"

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
             const int margin) :
        mName(name),
        mX(x), mY(y),
        mWidth(w), mHeight(h),
        mMargin(margin),
        mMap(map),
        mSettings(settings) {}

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
        std::vector<eRect> terrainRects;
        for(const auto& c : mConnecitons) {
            const eRect connRect{c.fX, c.fY, c.fW, c.fH};
            eRect connIn;
            eRect::intersection(rect, connRect, connIn);
            if(connIn.fW <= 0 || connIn.fH <= 0) continue;
            terrainRects.emplace_back(connIn);
        }

        bool rectWalls = false;
        bool fillEmptySapces = false;

        switch(mSettings.fType) {
        case eAreaType::dungeon: {
            rectWalls = true;
            fillEmptySapces = false;
            const auto in = rect.inset(mMargin);
            terrainRects.emplace_back(in);
        } break;
        case eAreaType::open: {
            rectWalls = false;
            fillEmptySapces = true;
            const auto in = rect.inset(mMargin);
            terrainRects.emplace_back(in);
        } break;
        }

        const auto inRect = [&terrainRects](const int x, const int y,
                                            const eRect* const skip = nullptr) {
            const ePoint p{x, y};
            for(const auto& rect : terrainRects) {
                if(&rect == skip) continue;
                const bool r = rect.contains(p);
                if(r) return true;
            }
            return false;
        };

        const auto shouldWall = [&](const int x, const int y,
                                    const eRect& srect,
                                    bool& wallTL, bool& wallTR) {
            wallTL = true;
            wallTR = true;
            const ePoint p{x, y};
            for(const auto& r : terrainRects) {
                if(&r == &srect) continue;
                const auto inside = [&r, &srect](const int x, const int y) {
                    return r.contains({x, y}) || srect.contains({x, y});
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
            [this](const eRect& rect,
                   const int x, const int y,
                   const int objMargin,
                   const std::vector<eTypeProbability>& objs) {
            if(objs.empty()) return;
            for(int dx = -objMargin; dx <= objMargin; dx++) {
                const int xx = x + dx;
                if(xx < rect.fX) return;
                if(xx >= rect.fX + rect.fW) return;
                for(int dy = -objMargin; dy <= objMargin; dy++) {
                    const int yy = y + dy;
                    if(yy < rect.fY) return;
                    if(yy >= rect.fY + rect.fH) return;
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

        const auto tryAddMonsterGroup =
            [this](const eRect& rect,
                   const int x, const int y,
                   const int monsterMargin,
                   const std::vector<eMonsterProbability>& ms) {
            if(ms.empty()) return;
            for(int dx = -monsterMargin; dx <= monsterMargin; dx++) {
                const int xx = x + dx;
                if(xx < rect.fX) return;
                if(xx >= rect.fX + rect.fW) return;
                for(int dy = -monsterMargin; dy <= monsterMargin; dy++) {
                    const int yy = y + dy;
                    if(yy < rect.fY) return;
                    if(yy >= rect.fY + rect.fH) return;
                    const auto& objs = mMap->objects(x + dx, y + dy);
                    if(!objs.empty()) return;
                }
            }
        };

        const auto& ms = mSettings.fMonsters;
        auto& mareas = mMap->mMonsterAreas;
        for(const auto& srect : terrainRects) {
            auto& marea = mareas.emplace_back();
            marea.fRect = srect;
            marea.fSettings = ms;

            const int minX = srect.fX;
            const int maxX = srect.fX + srect.fW - 1;
            const int minY = srect.fY;
            const int maxY = srect.fY + srect.fH - 1;
            for(int x = minX; x <= maxX + 1; x++) {
                for(int y = minY; y <= maxY + 1; y++) {
                    auto& dst = tiles[y][x];
                    if(x <= maxX && y <= maxY) {
                        setTileTerrain(x, y, dst);
                    }

                    const int m = mSettings.fObjectsMargin;
                    const auto& objs = mSettings.fObjects;
                    tryAddObject(srect, x, y, m, objs);

                    bool wallTL = true;
                    bool wallTR = true;
                    if(!rectWalls) {
                        shouldWall(x, y, srect, wallTL, wallTR);
                    }

                    if(wallTL && !dst.fWallTL) {
                        if(x == minX && y != maxY + 1) {
                            dst.fWallTL = eTile::encodeWall(true, false, false, 0);
                        }
                        if(x == maxX + 1 && y != maxY + 1) {
                            dst.fWallTL = eTile::encodeWall(true, false, false, 0);
                        }
                    }
                    if(wallTR && !dst.fWallTR) {
                        if(y == minY && x != maxX + 1) {
                            dst.fWallTR = eTile::encodeWall(true, false, false, 0);
                        }
                        if(y == maxY + 1 && x != maxX + 1) {
                            dst.fWallTR = eTile::encodeWall(true, false, false, 0);
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
                    tryAddObject(rect, x, y, m, objs);
                }
            }
        }
    }

    void generateWalls() const {
        const auto rect = eDungeon::rect();
        const auto terrType = mSettings.fTerrainType;
        const auto& terrTypeInfo = eTerrsTexturesData::get(terrType);
        const auto& tlWalls = terrTypeInfo.fTLWalls;
        const auto& trWalls = terrTypeInfo.fTRWalls;
        auto& tiles = mMap->mTiles;
        const int h = mMap->height();
        const int w = mMap->width();
        const auto hasTLWall = [w, h, &tiles](const int x, const int y) {
            if(x < 0 || y < 0 || x >= w || y >= h) return false;
            const auto& src = tiles[y][x];
            return !!src.fWallTL;
        };

        const auto has0TLWall = [&tiles](const int x, const int y) {
            const auto& src = tiles[y][x];
            if(!src.fWallTL) return false;
            bool b;
            uint8_t type;
            eTile::decodeWall(src.fWallTL, b, b, b, type);
            return type == 0;
        };

        const int maxTLSizeClamp = tlWalls.fSizes.back();
        const auto maxTLSize = [&](const int x, const int y) {
            for(int i = 0; i < maxTLSizeClamp; i++) {
                const bool r = hasTLWall(x, y + i);
                if(!r) return i;
            }
            return maxTLSizeClamp;
        };

        const auto chooseTLVec = [&](const int maxSize) {
            const auto& d = tlWalls.fDataIds;
            for(int i = d.size() - 1; i >= 0; i--) {
                const auto& v = d[i];
                if(v.size() <= maxSize) return v;
            }
            return d[0];
        };

        const auto hasTRWall = [w, h, &tiles](const int x, const int y) {
            if(x < 0 || y < 0 || x >= w || y >= h) return false;
            const auto& src = tiles[y][x];
            return !!src.fWallTR;
        };

        const auto has0TRWall = [&tiles](const int x, const int y) {
            const auto& src = tiles[y][x];
            if(!src.fWallTR) return false;
            bool b;
            uint8_t type;
            eTile::decodeWall(src.fWallTR, b, b, b, type);
            return type == 0;
        };

        const int maxTRSizeClamp = trWalls.fSizes.back();
        const auto maxTRSize = [&](const int x, const int y) {
            for(int i = 0; i < maxTRSizeClamp; i++) {
                const bool r = hasTRWall(x + i, y);
                if(!r) return i;
            }
            return maxTRSizeClamp;
        };

        const auto chooseTRVec = [&](const int maxSize) {
            const auto& d = trWalls.fDataIds;
            for(int i = d.size() - 1; i >= 0; i--) {
                const auto& v = d[i];
                if(v.size() <= maxSize) return v;
            }
            return d[0];
        };

        for(int x = rect.fX; x < rect.fX + rect.fW; x++) {
            for(int y = rect.fY; y < rect.fY + rect.fH; y++) {
                {
                    const bool r = has0TLWall(x, y);
                    if(r) {
                        const int maxSize = maxTLSize(x, y);
                        const auto v = chooseTLVec(maxSize);
                        const int size = v.size();
                        for(int dy = 0; dy < size; dy++) {
                            auto& dst = tiles[y + dy][x];
                            dst.fWallTL = eTile::encodeWall(true, false, false, v[dy]);
                        }
                    }
                }
                {
                    const bool r = has0TRWall(x, y);
                    if(r) {
                        const int maxSize = maxTRSize(x, y);
                        const auto v = chooseTRVec(maxSize);
                        const int size = v.size();
                        for(int dx = 0; dx < size; dx++) {
                            auto& dst = tiles[y][x + dx];
                            dst.fWallTR = eTile::encodeWall(true, false, false, v[dx]);
                        }
                    }
                }
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
    eMapSettings::load();
    const auto it = eMapSettings::sMaps.find(name);
    if(it == eMapSettings::sMaps.end()) {
        eRuntimeThrow("No map \"" + name + "\" settings found.");
    }
    const auto& mapSettings = it->second;
    if(mapSettings.fAreas.size() == 0) {
        eRuntimeThrow("No areas to generate for \"" + name + "\"");
    }
    const auto result = std::make_shared<eMap>();

    const int areaDim = 80;

    std::map<eAreaPlace, eDungeon> areas;

    eAreaPlacer placer(areaDim);
    const auto firstPlace = placer.iniPlace();

    std::function<eAreaPlace(const std::string& name,
                             const eAreaSettings& settings,
                             const eAreaPlace& nextTo)> genArea;
    const int connWidth = 4;
    const int connHalfLen = 4;
    genArea = [&](const std::string& name,
                  const eAreaSettings& settings,
                  const eAreaPlace& nextTo) {
        const auto place = placer.choosePlace(nextTo);
        const auto pos = placer.pos(place);

        const int x = pos.fX;
        const int y = pos.fY;

        auto& area = areas[place];
        area = eDungeon(name, x, y, areaDim, areaDim,
                        result, settings, connHalfLen);

        for(const auto& conn : settings.fConnections) {
            const auto connType = conn.second;
            if(connType != eConnectionType::plain) continue;
            const auto name = conn.first;
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

    const uint16_t grassId = eTerrsTexturesData::id("grass");
    result->mTerrainTypes.emplace(grassId);

    const uint16_t basementId = eTerrsTexturesData::id("basement");
    result->mTerrainTypes.emplace(basementId);

    const auto townFenceId = eObjectsInfo::sObjects.id("town_fence");
    result->mObjectTypes.emplace(townFenceId);
    const auto& townFenceInfo = eObjectsInfo::sObjects.get(townFenceId);

    const auto treeId = eObjectsInfo::sObjects.id("tree");
    result->mObjectTypes.emplace(treeId);
    const auto& treeInfo = eObjectsInfo::sObjects.get(treeId);

    const auto chestId = eObjectsInfo::sObjects.id("chest");
    result->mObjectTypes.emplace(chestId);
    const auto& chestInfo = eObjectsInfo::sObjects.get(chestId);

    const auto smallChestId = eObjectsInfo::sObjects.id("small_chest");
    result->mObjectTypes.emplace(smallChestId);
    const auto& smallChestInfo = eObjectsInfo::sObjects.get(smallChestId);

    result->generateTiles(rect.fW + 2*extMargin + 1,
                          rect.fH + 2*extMargin + 1);
    bool first = true;
    for(const auto& it : areas) {
        const auto& area = it.second;
        const auto& name = area.name();
        eMapArea mapArea;
        const int id = mapSettings.fAreas.id(name);
        const auto& sett = mapSettings.fAreas.get(id);
        mapArea.fLightness = sett.fLightness;
        mapArea.fContrast = sett.fContrast;
        const auto rect = area.rect();
        mapArea.fRect = rect;
        result->mAreas.add(name, mapArea);
        area.generate();
        if(first) {
            first = false;
            result->mSpawnPos = ePoint{rect.fX + rect.fW/2,
                                       rect.fY + rect.fH/2};
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

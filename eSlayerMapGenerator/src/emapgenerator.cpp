#include "eSlayerMapGenerator/emapgenerator.h"

#include "emapsettings.h"

#include <eSlayerHelpers/epacket.h>
#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/eobjstexturesdata.h>
#include <eSlayerHelpers/echardatainfo.h>
#include <eSlayerHelpers/erect.h>

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
    updateObjectsMap();
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

// --------------------------------------------------
// Map
// --------------------------------------------------
enum Tile {
    WALL,
    FLOOR
};

// --------------------------------------------------
// Union-Find (for clustering + MST)
// --------------------------------------------------
struct eUnionFind {
    std::vector<int> parent, rank;

    eUnionFind(int n) : parent(n), rank(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }

    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }

    void unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return;

        if (rank[a] < rank[b])
            parent[a] = b;
        else {
            parent[b] = a;
            if (rank[a] == rank[b])
                rank[a]++;
        }
    }
};

// --------------------------------------------------
// eDungeon Generator
// --------------------------------------------------
class eDungeon {
public:
    int fX, fY;
    int width, height;
    std::vector<std::vector<Tile>> map;
    std::vector<eRect> rooms;
    std::vector<std::vector<eRect>> areas;

    eDungeon(const int x, const int y,
             const int w, const int h) :
        fX(x), fY(y), width(w), height(h) {
        map.resize(width, std::vector<Tile>(height, WALL));
    }

    eDungeon() {}

    eRect randomRoom() {
        const int w = eRand::rand(10, 20);
        const int h = eRand::rand(10, 20);
        const int x = eRand::rand(1, width - w - 2);
        const int y = eRand::rand(1, height - h - 2);
        return {x, y, w, h};
    }

    void generateRooms(int count) {
        for(int i = 0; i < count; ++i) {
            rooms.push_back(randomRoom());
        }
    }

    void clusterRooms() {
        eUnionFind uf(rooms.size());

        for(size_t i = 0; i < rooms.size(); ++i) {
            for(size_t j = i + 1; j < rooms.size(); ++j) {
                if(eRect::intersects(rooms[i], rooms[j])) {
                    uf.unite(i, j);
                }
            }
        }

        // Group into areas
        std::unordered_map<int, std::vector<eRect>> groups;
        for(size_t i = 0; i < rooms.size(); ++i) {
            groups[uf.find(i)].push_back(rooms[i]);
        }

        for(auto& [_, group] : groups) {
            areas.push_back(group);
        }
    }

    void carveRoom(const eRect& r) {
        for(int x = r.fX; x < r.fX + r.fW; ++x) {
            for(int y = r.fY; y < r.fY + r.fH; ++y) {
                map[x][y] = FLOOR;
            }
        }
    }

    void carveRooms() {
        for(auto& r : rooms) {
            carveRoom(r);
        }
    }

    ePoint areaCenter(const std::vector<eRect>& area) {
        int sumX = 0, sumY = 0;
        for(auto& r : area) {
            sumX += r.centerX();
            sumY += r.centerY();
        }
        return { sumX / (int)area.size(), sumY / (int)area.size() };
    }

    void carveCorridor(const ePoint& a, const ePoint& b) {
        int x = a.fX;
        int y = a.fY;

        if(eRand::randChance(0.5f)) {
            // horizontal first
            while(x != b.fX) {
                map[x][y] = FLOOR;
                map[x][y + 1] = FLOOR;
                x += (b.fX > x) ? 1 : -1;
            }
            while(y != b.fY) {
                map[x][y] = FLOOR;
                map[x][y + 1] = FLOOR;
                y += (b.fY > y) ? 1 : -1;
            }
        } else {
            // vertical first
            while(y != b.fY) {
                map[x][y] = FLOOR;
                map[x + 1][y] = FLOOR;
                y += (b.fY > y) ? 1 : -1;
            }
            while(x != b.fX) {
                map[x][y] = FLOOR;
                map[x + 1][y] = FLOOR;
                x += (b.fX > x) ? 1 : -1;
            }
        }
    }

    void connectAreas() {
        if(areas.size() <= 1) return;

        std::vector<ePoint> centers;
        for(const auto& area : areas) {
            centers.push_back(areaCenter(area));
        }

        struct eEdge {
            int a, b;
            double dist;
        };

        std::vector<eEdge> edges;

        for(size_t i = 0; i < centers.size(); ++i) {
            for(size_t j = i + 1; j < centers.size(); ++j) {
                double d = hypot(centers[i].fX - centers[j].fX,
                                 centers[i].fY - centers[j].fY);
                edges.push_back({(int)i, (int)j, d});
            }
        }

        sort(edges.begin(), edges.end(), [](const eEdge& a, const eEdge& b) {
            return a.dist < b.dist;
        });

        eUnionFind uf(centers.size());

        for(const auto& e : edges) {
            if(uf.find(e.a) != uf.find(e.b)) {
                uf.unite(e.a, e.b);
                carveCorridor(centers[e.a], centers[e.b]);
            }
        }

        // Optional: add extra loops
        for(const auto& e : edges) {
            if(eRand::randChance(0.1)) {
                carveCorridor(centers[e.a], centers[e.b]);
            }
        }
    }

    std::pair<ePoint, ePoint> closestPointsBetweenDungeons(const eDungeon& other) {
        double bestDist = std::numeric_limits<double>::max();
        ePoint bestA, bestB;

        for(const auto& ra : rooms) {
            for(const auto& rb : other.rooms) {

                ePoint a = {
                    fX + ra.centerX(),
                    fY + ra.centerY()
                };

                ePoint b = {
                    other.fX + rb.centerX(),
                    other.fY + rb.centerY()
                };

                double d = hypot(a.fX - b.fX, a.fY - b.fY);

                if(d < bestDist) {
                    bestDist = d;
                    bestA = a;
                    bestB = b;
                }
            }
        }

        return {bestA, bestB};
    }

    void carveCorridorTo(eDungeon& other) {
        auto [start, end] = closestPointsBetweenDungeons(other);

        int x = start.fX;
        int y = start.fY;

        auto carveAt = [&](int wx, int wy) {
            // Check this dungeon
            if(wx >= fX && wx < fX + width &&
               wy >= fY && wy < fY + height) {
                map[wx - fX][wy - fY] = FLOOR;
            }

            // Check other dungeon
            if(wx >= other.fX && wx < other.fX + other.width &&
                wy >= other.fY && wy < other.fY + other.height) {
                other.map[wx - other.fX][wy - other.fY] = FLOOR;
            }
        };

        if(eRand::randChance(0.5f)) {
            while(x != end.fX) {
                carveAt(x, y);
                carveAt(x, y + 1);
                x += (end.fX > x) ? 1 : -1;
            }
            while(y != end.fY) {
                carveAt(x, y);
                carveAt(x + 1, y);
                y += (end.fY > y) ? 1 : -1;
            }
        } else {
            while(y != end.fY) {
                carveAt(x, y);
                carveAt(x + 1, y);
                y += (end.fY > y) ? 1 : -1;
            }
            while(x != end.fX) {
                carveAt(x, y);
                carveAt(x, y + 1);
                x += (end.fX > x) ? 1 : -1;
            }
        }

        carveAt(end.fX, end.fY);
    }
};

enum class eDir {
    topLeft, bottomRight,
    topRight, bottomLeft
};

struct eAreaPlace {
    int fX;
    int fY;
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
        return {place.fX*mAreaDim, place.fY*mAreaDim};
    }

    const eRect& boundingRect() const {
        return mBoundingRect;
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

            if(!mUsedAreas[usedX][usedY]) {
                mUsedAreas[usedX][usedY] = true;
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

    std::map<std::string, eDungeon> areas;

    eAreaPlacer placer(areaDim);
    const auto firstPlace = placer.iniPlace();

    std::function<void(const std::string& name,
                       const eAreaSettings& settings,
                       const eAreaPlace& nextTo)> genArea;
    genArea = [&](const std::string& name,
                  const eAreaSettings& settings,
                  const eAreaPlace& nextTo) {
        const auto place = placer.choosePlace(nextTo);
        const auto pos = placer.pos(place);

        const int x = pos.fX;
        const int y = pos.fY;

        auto& area = areas[name];
        area = eDungeon(x, y, areaDim, areaDim);

        area.generateRooms(20);
        area.clusterRooms();
        area.carveRooms();
        area.connectAreas();

        for(const auto& conn : settings.fConnections) {
            const auto connType = conn.second;
            if(connType != eConnectionType::plain) continue;
            const auto name = conn.first;
            const int settingsId = mapSettings.fAreas.id(name);
            const auto settings = mapSettings.fAreas.get(settingsId);
            genArea(name, settings, place);
            auto& newArea = areas[name];
            area.carveCorridorTo(newArea);
        }
    };

    const auto name0 = mapSettings.fAreas.name(0);
    const eAreaSettings& settings = mapSettings.fAreas.get(0);
    genArea(name0, settings, firstPlace);

    auto rect = placer.boundingRect();
    for(auto& it : areas) {
        auto& area = it.second;
        area.fX -= rect.fX;
        area.fY -= rect.fY;
    }
    rect.fX = 0;
    rect.fY = 0;

    const uint16_t townFloorId = eTerrsTexturesData::id("grass");
    result->mTerrainTypes.emplace(townFloorId);

    const auto townFenceId = eObjsTexturesData::id("town_fence");
    result->mObjectTypes.emplace(townFenceId);

    result->generateTiles(rect.fW, rect.fH);
    for(const auto& it : areas) {
        const auto& name = it.first;
        const auto& area = it.second;
        eMapArea mapArea;
        const int id = mapSettings.fAreas.id(name);
        const auto& sett = mapSettings.fAreas.get(id);
        mapArea.fLightness = sett.fLightness;
        mapArea.fContrast = sett.fContrast;
        mapArea.fRect = eRect{area.fX, area.fY,
                              area.width, area.height};
        result->mAreas.add(name, mapArea);
        for(int x = 0; x < area.width; x++) {
            for(int y = 0; y < area.height; y++) {
                const int globalX = x + area.fX;
                const int globalY = y + area.fY;
                auto& dst = result->mTiles[globalY][globalX];
                const auto& src = area.map[x][y];
                dst.fTerrainType = 1;
                dst.fTileType = eRand::rand() % 20;
                if(src == Tile::WALL) {
                    auto& obj = result->mObjects.emplace_back();
                    obj.fObjectType = 0;
                    obj.fTileType = 0;
                    obj.fTileX = globalX;
                    obj.fTileY = globalY;
                } else if(result->mSpawnPos == ePoint{0, 0}) {
                    result->mSpawnPos = {globalX, globalY};
                }
            }
        }
    }

    result->updateObjectsMap();

    result->mUnitTypes = settings.fMonsters;

    return result;
}

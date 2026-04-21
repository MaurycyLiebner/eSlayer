#include "eSlayerMapGenerator/emapgenerator.h"

#include "emapsettings.h"

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
    data.fLight = mLight;
    data.fContrast = mContrast;
    data.fSpawnPos = mSpawnPos;
}

void eMap::loadData(const eMapData& data) {
    generateTiles(data.fTotalWidth, data.fTotalHeight);
    mTerrainTypes = data.fTerrainTypes;
    mObjectTypes = data.fObjectTypes;
    mUnitTypes = data.fUnitTypes;
    mLight = data.fLight;
    mContrast = data.fContrast;
    mSpawnPos = data.fSpawnPos;
    updateObjectsMap();
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

struct eRect {
    int x, y, w, h;

    int centerX() const { return x + w / 2; }
    int centerY() const { return y + h / 2; }
};

bool intersects(const eRect& a, const eRect& b) {
    return !(a.x + a.w < b.x || b.x + b.w < a.x ||
             a.y + a.h < b.y || b.y + b.h < a.y);
}

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
    int width, height;
    std::vector<std::vector<Tile>> map;
    std::vector<eRect> rooms;
    std::vector<std::vector<eRect>> areas;

    eDungeon(const int w, const int h) : width(w), height(h) {
        map.resize(width, std::vector<Tile>(height, WALL));
    }

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
                if(intersects(rooms[i], rooms[j])) {
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
        for(int x = r.x; x < r.x + r.w; ++x) {
            for(int y = r.y; y < r.y + r.h; ++y) {
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
        if (areas.size() <= 1) return;

        std::vector<ePoint> centers;
        for(auto& area : areas) {
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

        for(auto& e : edges) {
            if(uf.find(e.a) != uf.find(e.b)) {
                uf.unite(e.a, e.b);
                carveCorridor(centers[e.a], centers[e.b]);
            }
        }

        // Optional: add extra loops
        for(auto& e : edges) {
            if(eRand::randChance(0.1)) {
                carveCorridor(centers[e.a], centers[e.b]);
            }
        }
    }
};

std::shared_ptr<eMap>
eMapGenerator::generate(const std::string& name) const {
    eMapSettings::load();
    const auto it = eMapSettings::sMaps.find(name);
    if(it == eMapSettings::sMaps.end()) {
        eRuntimeThrow("No map \"" + name + "\" settings found.");
    }
    const auto& settings = it->second;
    const auto result = std::make_shared<eMap>();
    if(name == "act1") {
        const uint16_t townFloorId = eTerrsTexturesData::id("grass");
        result->mTerrainTypes.emplace(townFloorId);

        const int w = 80;
        const int h = 80;

        eDungeon dungeon(w, h);

        dungeon.generateRooms(20);
        dungeon.clusterRooms();
        dungeon.carveRooms();
        dungeon.connectAreas();

        const auto townFenceId = eObjsTexturesData::id("town_fence");
        result->mObjectTypes.emplace(townFenceId);

        result->generateTiles(w, h);
        for(int x = 0; x < w; x++) {
            for(int y = 0; y < h; y++) {
                auto& dst = result->mTiles[y][x];
                const auto& src = dungeon.map[x][y];
                dst.fTerrainType = 1;
                dst.fTileType = eRand::rand() % 20;
                if(src == Tile::WALL) {
                    auto& obj = result->mObjects.emplace_back();
                    obj.fObjectType = 0;
                    obj.fTileType = 0;
                    obj.fTileX = x;
                    obj.fTileY = y;
                } else if(result->mSpawnPos == ePoint{0, 0}) {
                    result->mSpawnPos = {x, y};
                }
            }
        }

        result->updateObjectsMap();

        result->mUnitTypes = settings.fMonsters;
        result->mLight = settings.fLightness;
        result->mContrast = settings.fContrast;
    }
    return result;
}

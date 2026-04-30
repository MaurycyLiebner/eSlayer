#include "eSlayerMapGenerator/emapgenerator.h"

#include "emapsettings.h"
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

    result->generateTiles(rect.fW, rect.fH);
    for(const auto& it : areas) {
        const auto& name = it.first;
        const bool isBloodMoor = name == "blood_moor";
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
                const auto terrType = isBloodMoor ? basementId : grassId;
                dst.fTerrainType = terrType;
                dst.fTileType = isBloodMoor ? 1 : 1 + eRand::rand() % 20;
                const auto& terrInfo = eTerrsTexturesData::get(terrType);
                if(src == Tile::WALL) {
                    bool inner = true;
                    for(int dx = -1; dx <= 1; dx++) {

                        const int srcX = x + dx;
                        if(srcX < 0) continue;
                        else if(srcX >= area.width) break;

                        const int dstX = globalX + dx;
                        if(dstX < 0) continue;
                        else if(dstX >= result->mWidth) break;

                        for(int dy = -1; dy <= 1; dy++) {
                            if(dx == 0 && dy == 0) continue;

                            const int srcY = y + dy;
                            if(srcY < 0) continue;
                            else if(srcY >= area.height) break;

                            const int dstY = globalY + dy;
                            if(dstY < 0) continue;
                            else if(dstY >= result->mHeight) break;

                            const auto& src = area.map[srcX][srcY];
                            if(src == Tile::FLOOR) {
                                inner = false;
                                break;
                            }
                        }
                        if(!inner) break;
                    }
                    if(inner) {
                        if(eRand::randChance(0.1f)) {
                            auto& obj = result->mObjects.emplace_back();
                            obj.fObjectType = treeId;
                            obj.fTileType = eRand::rand();
                            obj.fPos.fX = globalX;
                            obj.fPos.fY = globalY;
                            obj.fSize = treeInfo.fSize;
                        }
                    } else {
                        if(isBloodMoor) {
                            dst.fTerrainType = basementId;
                            std::vector<std::vector<bool>> walls(
                                3, std::vector<bool>(3, true));
                            for(int dx = -1; dx <= 1; dx++) {

                                const int srcX = x + dx;
                                if(srcX < 0) continue;
                                else if(srcX >= area.width) break;

                                const int dstX = globalX + dx;
                                if(dstX < 0) continue;
                                else if(dstX >= result->mWidth) break;

                                for(int dy = -1; dy <= 1; dy++) {
                                    if(dx == 0 && dy == 0) continue;

                                    const int srcY = y + dy;
                                    if(srcY < 0) continue;
                                    else if(srcY >= area.height) break;

                                    const int dstY = globalY + dy;
                                    if(dstY < 0) continue;
                                    else if(dstY >= result->mHeight) break;

                                    const auto& src = area.map[srcX][srcY];
                                    walls[dy + 1][dx + 1] = src == Tile::WALL;
                                }
                            }

                            const std::vector<int>* options = nullptr;
                            if(!walls[2][1] && !walls[1][2]) {
                                options = &terrInfo.fBBorders;
                            } else if(!walls[1][0] && !walls[0][1]) {
                                options = &terrInfo.fTBorders;
                            } else if(!walls[0][1] && !walls[1][2]) {
                                options = &terrInfo.fRBorders;
                            } else if(!walls[1][0] && !walls[2][1]) {
                                options = &terrInfo.fLBorders;
                            } else if(!walls[2][1]) {
                                options = &terrInfo.fBLBorders;
                            } else if(!walls[1][2]) {
                                options = &terrInfo.fBRBorders;
                            } else if(!walls[1][0]) {
                                options = &terrInfo.fTLBorders;
                            } else if(!walls[0][1]) {
                                options = &terrInfo.fTRBorders;
                            }
                            if(options) {
                                const int n = options->size();
                                const int id = eRand::rand() % n;
                                dst.fTileType = (*options)[id];
                            }
                        } else {
                            auto& obj = result->mObjects.emplace_back();
                            obj.fObjectType = townFenceId;
                            obj.fTileType = 0;
                            obj.fPos.fX = globalX;
                            obj.fPos.fY = globalY;
                            obj.fSize = townFenceInfo.fSize;
                        }
                    }
                } else if(result->mSpawnPos == ePoint{0, 0}) {
                    result->mSpawnPos = {globalX, globalY};
                } else {
                    if(eRand::randChance(0.025f)) {
                        auto& obj = result->mObjects.emplace_back();
                        obj.fObjectType = treeId;
                        obj.fTileType = eRand::rand();
                        obj.fPos.fX = globalX;
                        obj.fPos.fY = globalY;
                        obj.fSize = treeInfo.fSize;
                    } else if(eRand::randChance(0.01f)) {
                        auto& obj = result->mObjects.emplace_back();
                        obj.fObjectType = eRand::randChance(0.5) ? chestId : smallChestId;
                        obj.fTileType = eRand::rand();
                        obj.fPos.fX = globalX;
                        obj.fPos.fY = globalY;
                        obj.fSize = chestInfo.fSize;
                    }
                }
            }
        }
    }

    result->updateObjectsMap();

    result->mUnitTypes = settings.fMonsters;

    return result;
}

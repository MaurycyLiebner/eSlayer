#include "edungeon.h"

#include "edungeongenerator.h"
#include "eopengenerator.h"
#include "ewallfinisher.h"

#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/eobjectsinfo.h>

eDungeon::eDungeon() {}

eDungeon::eDungeon(const std::string& name,
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

void eDungeon::addConnection(const eConnection& conn) {
    mConnecitons.emplace_back(conn);
}

void eDungeon::shift(const int dx, const int dy) {
    mX += dx;
    mY += dy;
    for(auto& conn : mConnecitons) {
        conn.fX += dx;
        conn.fY += dy;
    }
}

eRect eDungeon::rect() const {
    return eRect{mX, mY, mWidth, mHeight};
}

void eDungeon::setExtendedRect(const eRect& rect) {
    mExtendedRect = rect;
}

void eDungeon::generate() const {
    const auto rect = eDungeon::rect();
    std::vector<eChamber> chambers;
    std::vector<eRect> doors;
    for(const auto& c : mConnecitons) {
        const eRect connRect{c.fX, c.fY, c.fW, c.fH};
        eRect connIn;
        eRect::intersection(rect, connRect, connIn);
        if(connIn.fW <= 0 || connIn.fH <= 0) {
            continue;
        }
        chambers.emplace_back(connIn);
    }

    bool rectWalls = false;
    bool fillEmptySapces = false;

    switch(mSettings.fType) {
    case eAreaType::dungeon: {
        rectWalls = true;
        fillEmptySapces = false;
        eDungeonGenerator::generate(rect, chambers, doors);
    } break;
    case eAreaType::open: {
        rectWalls = false;
        fillEmptySapces = true;
        eOpenGenerator::generate(rect, chambers, doors, mMargin);
    } break;
    }

    const auto inRect = [&](const int x, const int y, const eChamber* const skip = nullptr) {
        const ePoint p{x, y};
        for (const auto& c : chambers) {
            if(&c == skip) continue;
            const bool r = c.contains(p);
            if(r) return true;
        }
        return false;
    };

    const auto inDoorsRect = [&](const int x, const int y, const eWallType type) {
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

    const auto shouldWall = [&](const int x, const int y, const eChamber& sc, bool& wallTL,
                                bool& wallTR) {
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

    const auto setTileTerrain = [&](const int x, const int y, eTile& dst) {
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

    const auto tryAddObject = [this](const eChamber& c, const int x, const int y,
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

void eDungeon::generateWalls() const {
    const auto rect = eDungeon::rect();
    eWallFinisher::finish(rect, mSettings, *mMap);
}

const std::string& eDungeon::name() const {
    return mName;
}

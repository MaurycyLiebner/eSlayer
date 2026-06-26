#include "edungeon.h"

#include "edungeongenerator.h"
#include "eopengenerator.h"
#include "ewallfinisher.h"

#include <eSlayerHelpers/eobjectsinfo.h>
#include <eSlayerHelpers/eplacementhelper.h>
#include <eSlayerHelpers/eterrstexturesdata.h>
#include <eSlayerHelpers/eblueprints.h>
#include <eSlayerHelpers/esellers.h>

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
    mConnections.emplace_back(conn);
}

void eDungeon::shift(const int dx, const int dy) {
    mX += dx;
    mY += dy;
    for(auto& conn : mConnections) {
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

void eDungeon::generate(ePointF& spawnPos) const {
    const auto rect = eDungeon::rect();
    std::vector<eChamber> chambers;
    std::vector<eRect> doors;
    for(const auto& c : mConnections) {
        const eRect connRect{c.fX, c.fY, c.fW, c.fH};
        eRect connIn;
        const bool r = eRect::intersection(rect, connRect, connIn);
        if(!r) continue;
        chambers.emplace_back(connIn);
    }

    bool rectWalls = false;
    bool fillEmptySapces = false;

    switch(mSettings.fType) {
    case eAreaType::dungeon: {
        rectWalls = true;
        fillEmptySapces = false;
        eDungeonGenerator::generate(rect, chambers, doors, mSettings.fSize);
    } break;
    case eAreaType::open: {
        rectWalls = false;
        fillEmptySapces = true;
        eOpenGenerator::generate(rect, chambers, doors, mMargin);
    } break;
    case eAreaType::camp: {
        rectWalls = false;
        fillEmptySapces = true;
        if(mConnections.size() != 1) {
            eRuntimeThrow("Camp should have exactly one connection.");
        }
        const auto& c = mConnections[0];
        eRect connIn;
        const eRect connRect{c.fX, c.fY, c.fW, c.fH};
        eRect::intersection(rect, connRect, connIn);
        const int size = mSettings.fSize;
        eRect crect;
        if(connIn.fX == rect.fX) { // top left
            crect = eRect{connIn.fX + connIn.fW,
                          connIn.fY + connIn.fH/2 - size/2,
                          size, size};
        } else if(connIn.fX + connIn.fW == rect.fX + rect.fW) { // bottom right
            crect = eRect{connIn.fX - size,
                          connIn.fY + connIn.fH/2 - size/2,
                          size, size};
        } else if(connIn.fY == rect.fY) { // top right
            crect = eRect{connIn.fX + connIn.fW/2 - size/2,
                          connIn.fY + connIn.fH,
                          size, size};
        } else { // bottom left
            crect = eRect{connIn.fX + connIn.fW/2 - size/2,
                          connIn.fY - size,
                          size, size};
        }
        spawnPos = crect.center();
        chambers.emplace_back(crect);
    } break;
    }

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

    const auto shouldWall = [&](
        const int x, const int y,
        const eChamber& sc,
        bool& wallTL,
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

    const auto calcArea = [&](const int x, const int y,
                              const eChamber& c) {
        int dim;
        for(dim = 0;; dim++) {
            for(int dx = -dim; dx <= dim; dx++) {
                const int xx = x + dx;
                for(int dy = -dim; dy <= dim; dy++) {
                    const int yy = y + dy;
                    if(std::abs(dx) != dim && std::abs(dy) != dim) continue;
                    if(!c.contains({xx, yy})) return dim;
                    const auto& objs = mMap->objects(xx, yy);
                    if(!objs.empty()) return dim;
                    const auto& tile = mMap->tile(xx, yy);
                    const auto& overlays = tile.fOverlays;
                    if(!overlays.empty()) return dim;
                }
            }
        }
        return dim;
    };

    const auto checkMargin = [&](const int x, const int y,
                                 const eChamber& c) {
        const int objMargin = 1;
        for(int dx = -objMargin; dx <= objMargin; dx++) {
            const int xx = x + dx;
            for(int dy = -objMargin; dy <= objMargin; dy++) {
                const int yy = y + dy;
                if(!c.contains({xx, yy})) return false;
                const auto& objs = mMap->objects(xx, yy);
                if(!objs.empty()) return false;
            }
        }
        return true;
    };

    const auto calcMaxArea = [&](const eChamber& c,
                                 int& maxA,
                                 int& xMax,
                                 int& yMax,
                                 const bool random) {
        maxA = 0;
        for(const auto& r : c.fRects) {
            for(int x = r.fX; x < r.fX + r.fW; x++) {
                for(int y = r.fY; y < r.fY + r.fH; y++) {
                    const bool r = checkMargin(x, y, c);
                    if(!r) continue;
                    const int a = calcArea(x, y, c);
                    if(a <= maxA) continue;
                    maxA = a;
                    xMax = x;
                    yMax = y;
                    if(random) {
                        xMax += eRand::rand(-a/2, a/2);
                        yMax += eRand::rand(-a/2, a/2);
                    }
                }
            }
        }
    };

    bool waypointAdded = false;

    std::vector<std::shared_ptr<eObject>> trapDoors;

    const auto addObject = [&](const float x, const float y,
                               const uint16_t type,
                               const uint8_t subtype = eRand::rand(0, 255)) {
        const auto& info = eObjectsInfo::sObjects.get(type);
        const auto objPtr = mMap->addObject({x, y});
        auto& obj = *objPtr;
        obj.fObjectType = type;
        obj.fSubtype = subtype;
        obj.fSize = info.fSize;

        if(info.fType == eObjectType::trapDoor) {
            trapDoors.emplace_back(objPtr);
            mMap->mTrapDoors.emplace_back(objPtr);
        }

        const auto addSeller = [&](const eSellerType type) {
            const auto id = obj.fObjectId;
            auto& s = eSellers::sSellers[id];
            s.fId = id;
            s.fLevel = mSettings.fLevel;
            s.fType = type;
            s.fMapId = mMap->id();
            s.fSellItemTypes = info.fItemTypes;
            s.fSellPotionTypes = info.fPotionTypes;
        };

        switch(info.fType) {
        case eObjectType::healer:
            addSeller(eSellerType::healer);
            break;
        case eObjectType::trader:
            addSeller(eSellerType::trader);
            break;
        case eObjectType::waypoint:
            waypointAdded = true;
            break;
        case eObjectType::spawnArea: {
            const float s = 0.5f*obj.fSize;
            mMap->mSpawnPos = obj.fPos + eVec2f{s, s};
        } break;
        case eObjectType::portalArea: {
            const float s = 0.5f*obj.fSize;
            mMap->mPortalSpawnPos = obj.fPos + eVec2f{s, s};
        } break;
        default:
            break;
        }
    };

    const auto tryAddBlueprint = [&](ePlacementHelper& helper,
                                     const std::vector<eChamber>& cs,
                                     const eBlueprintCount& bpc) {
        if(cs.empty()) return false;
        int area;
        const int id = helper.get(area);
        if(id < 0) return false;
        const auto& c = cs[id];
        const auto& rects = c.fRects;
        if(rects.empty()) return false;
        const auto& r0 = rects[0];

        int maxA = 0;
        int xMax = r0.fX;
        int yMax = r0.fY;

        calcMaxArea(c, maxA, xMax, yMax, false);
        if(maxA == 0) return false;

        const auto& bp = eBlueprints::sBlueprints.get(bpc.fType);

        xMax -= bp.fWidth/2;
        yMax -= bp.fHeight/2;

        for(const auto& o : bp.fTerrain) {
            const auto& info = eTerrsTexturesData::get(o.fType);
            uint8_t tileType = 1;
            const int x0 = xMax + o.fX;
            const int y0 = yMax + o.fY;
            for(int dy = 0; dy < info.fHeight; dy++) {
                for(int dx = 0; dx < info.fWidth; dx++) {
                    auto& tile = mMap->tile(x0 + dx, y0 + dy);
                    auto& os = tile.fOverlays;
                    os.emplace_back(o.fType, tileType++);
                }
            }
        }

        for(const auto& o : bp.fObjects) {
            addObject(xMax + o.fX, yMax + o.fY, o.fType, o.fSubtype);
        }

        calcMaxArea(c, maxA, xMax, yMax, false);
        helper.set(id, maxA);

        return true;
    };

    const auto tryAddObject = [&](ePlacementHelper& helper,
                                  const std::vector<eChamber>& cs,
                                  const eObjectCount& os) {
        if(cs.empty()) return false;
        int area;
        const int id = helper.get(area);
        if(id < 0) return false;
        if(area < os.fMinArea) return false;
        const auto& c = cs[id];
        const auto& rects = c.fRects;
        if(rects.empty()) return false;
        const auto& r0 = rects[0];

        int maxA = 0;
        int xMax = r0.fX;
        int yMax = r0.fY;

        calcMaxArea(c, maxA, xMax, yMax, true);
        if(maxA == 0) return false;
        const auto type = os.fType;
        addObject(xMax, yMax, type);

        calcMaxArea(c, maxA, xMax, yMax, true);
        helper.set(id, maxA);

        return true;
    };

    const auto& ms = mSettings.fMonsters;

    auto& mareas = mMap->mMonsterAreas;
    auto& marea = mareas.emplace_back();
    marea.fChambers = chambers;
    marea.fSettings = ms;

    ePlacementHelper helper;
    for(int i = 0; i < chambers.size(); i++) {
        const auto& sc = chambers[i];
        int maxA = 0;
        int xMax;
        int yMax;
        calcMaxArea(sc, maxA, xMax, yMax, true);
        helper.add(i, maxA);
        for(const auto& rect : sc.fRects) {
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

    const auto& bps = mSettings.fBlueprints;
    for(const auto& bp : bps) {
        for(int i = 0; i < bp.fCount; i++) {
            const bool r = tryAddBlueprint(helper, chambers, bp);
            if(!r) break;
        }
    }

    const auto& objs = mSettings.fObjects;
    if(mSettings.fWaypoint && !waypointAdded) {
        const auto id = eObjectsInfo::sObjects.id("waypoint");
        const eObjectCount os(id, 1, 2);
        tryAddObject(helper, chambers, os);
    }
    for(const auto& os : objs) {
        for(int i = 0; i < os.fCount; i++) {
            const bool r = tryAddObject(helper, chambers, os);
            if(!r) break;
        }
    }

    if(fillEmptySapces) {
        std::vector<eRect> allRects;
        for(const auto& c : chambers) {
            const auto& cr = c.fRects;
            allRects.insert(allRects.end(), cr.begin(), cr.end());
        }
        const auto rects = eRect::subtractAll(mExtendedRect, allRects);
        ePlacementHelper helper;
        std::vector<eChamber> ochambers;
        for(const auto& r : rects) {
            for(int x = r.fX; x < r.fX + r.fW; x++) {
                for(int y = r.fY; y < r.fY + r.fH; y++) {
                    auto& dst = tiles[y][x];
                    setTileTerrain(x, y, dst);
                }
            }

            auto& c = ochambers.emplace_back();
            c.fRects = {r};
            int maxA = 0;
            int xMax;
            int yMax;
            calcMaxArea(c, maxA, xMax, yMax, true);
            const int id = ochambers.size() - 1;
            helper.add(id, maxA);
        }
        const auto& objs = mSettings.fOutsideObjects;
        for(const auto& os : objs) {
            for(int i = 0; i < os.fCount; i++) {
                const bool r = tryAddObject(helper, ochambers, os);
                if(!r) break;
            }
        }
    }

    std::vector<int> trapConns;
    for(const auto& it : mSettings.fConnections) {
        const auto& name = it.first;
        const auto& c = it.second;
        if(c.fType != eConnectionType::trapDoor) continue;
        const int mapId = eMapsSettings::sMaps.id(name);
        if(mapId < 0) {
            eRuntimeThrow("Unrecognized connection map name \"" + name + "\".");
        }
        trapConns.emplace_back(mapId);
    }

    if(trapDoors.size() != trapConns.size()) {
        eRuntimeThrow("Trap door count mismatch in \"" + mSettings.fName + "\".");
    }

    for(int i = 0; i < trapDoors.size(); i++) {
        const auto& o = trapDoors[i];
        o->fTargetMapId = trapConns[i];
    }
}

void eDungeon::generateWalls() const {
    const auto rect = eDungeon::rect();
    eWallFinisher::finish(rect, mSettings, *mMap);
}

const std::string& eDungeon::name() const {
    return mName;
}

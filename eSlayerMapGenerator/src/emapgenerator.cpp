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

struct eConnection {
    int fX;
    int fY;
    int fW;
    int fH;
};

class eDungeon {
public:
    eDungeon() {}
    eDungeon(const int x, const int y,
             const int w, const int h,
             const std::shared_ptr<eMap>& map,
             const eAreaSettings& settings,
             const int margin) :
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

    void generate() const {
        std::vector<eRect> terrainRects;
        for(const auto& c : mConnecitons) {
            const auto rect = eDungeon::rect();
            const eRect connRect{c.fX, c.fY, c.fW, c.fH};
            eRect connIn;
            eRect::intersection(rect, connRect, connIn);
            if(connIn.fW <= 0 || connIn.fH <= 0) continue;
            terrainRects.emplace_back(connIn);
        }
        switch(mSettings.fType) {
        case eAreaType::dungeon: {
            const auto rect = eDungeon::rect();
            const auto in = rect.inset(mMargin);
            terrainRects.emplace_back(in);
        } break;
        case eAreaType::open: {
            const auto rect = eDungeon::rect();
            const auto in = rect.inset(mMargin);
            terrainRects.emplace_back(in);
        } break;
        }

        const auto terrType = mSettings.fTerrainType;
        const auto& terrTypeInfo = eTerrsTexturesData::get(terrType);
        const auto floorUse = terrTypeInfo.fFloorUse;
        const auto& floor = terrTypeInfo.fFloor;
        auto& tiles = mMap->mTiles;
        for(const auto& rect : terrainRects) {
            for(int x = rect.fX; x < rect.fX + rect.fW; x++) {
                for(int y = rect.fY; y < rect.fY + rect.fH; y++) {
                    auto& dst = tiles[y][x];
                    dst.fWallTL = 0;
                    dst.fWallTR = 0;
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
                }
            }
        }
    }
private:
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

    std::map<std::string, eDungeon> areas;

    eAreaPlacer placer(areaDim);
    const auto firstPlace = placer.iniPlace();

    std::function<eAreaPlace(const std::string& name,
                             const eAreaSettings& settings,
                             const eAreaPlace& nextTo)> genArea;
    const int connWidth = 2;
    const int connHalfLen = 2;
    genArea = [&](const std::string& name,
                  const eAreaSettings& settings,
                  const eAreaPlace& nextTo) {
        const auto place = placer.choosePlace(nextTo);
        const auto pos = placer.pos(place);

        const int x = pos.fX;
        const int y = pos.fY;

        auto& area = areas[name];
        area = eDungeon(x, y, areaDim, areaDim,
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
            auto& connArea = areas[name];
            connArea.addConnection(conn_);
        }

        return place;
    };

    const auto name0 = mapSettings.fAreas.name(0);
    const eAreaSettings& settings = mapSettings.fAreas.get(0);
    genArea(name0, settings, firstPlace);

    auto rect = placer.boundingRect();
    for(auto& it : areas) {
        auto& area = it.second;
        area.shift(-rect.fX, -rect.fY);
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

    result->generateTiles(rect.fW + 1, rect.fH + 1);
    bool first = true;
    for(const auto& it : areas) {
        const auto& name = it.first;
        const auto& area = it.second;
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

    result->updateObjectsMap();

    result->mUnitTypes = settings.fMonsters;

    return result;
}

#include "eSlayerMapGenerator/emapgenerator.h"

#include "edungeon.h"

#include "eSlayerMapGenerator/emapsettings.h"
#include "eSlayerMapGenerator/emap.h"

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

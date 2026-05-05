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
};

class eDungeon {
public:
    eDungeon() {}
    eDungeon(const int x, const int y,
             const int w, const int h,
             const std::shared_ptr<eMap>& map,
             const eAreaSettings& settings) :
        mX(x), mY(y),
        mWidth(w), mHeight(h),
        mMap(map),
        mSettings(settings) {}

    void addConnection(const eConnection& conn) {
        mConnecitons.emplace_back(conn);
    }

    void shift(const int dx, const int dy) {
        mX += dx;
        mY += dy;
    }

    eRect rect() const {
        return eRect{mX, mY, mWidth, mHeight};
    }

    void generate() const {
        const auto terrType = mSettings.fTerrainType;
        auto& tiles = mMap->mTiles;
        for(int dx = 0; dx < mWidth; dx++) {
            for(int dy = 0; dy < mHeight; dy++) {
                const int globalX = mX + dx;
                const int globalY = mY + dy;
                auto& dst = tiles[globalY][globalX];
                dst.fWallTL = 0;
                dst.fWallTR = 0;
                dst.fTerrainType = terrType;
                if(mSettings.fType == eAreaType::dungeon) {
                    const int nTypes = 4;
                    const int dim = sqrt(nTypes);
                    dst.fTileType = 1 + (globalX % dim) + (globalY % dim) * dim;
                } else {
                    dst.fTileType = eRand::rand(1, 20);
                }
            }
        }
    }
private:
    int mX;
    int mY;
    int mWidth;
    int mHeight;

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
        area = eDungeon(x, y, areaDim, areaDim,
                        result, settings);

        for(const auto& conn : settings.fConnections) {
            const auto connType = conn.second;
            if(connType != eConnectionType::plain) continue;
            const auto name = conn.first;
            const int settingsId = mapSettings.fAreas.id(name);
            const auto settings = mapSettings.fAreas.get(settingsId);
            genArea(name, settings, place);
        }
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

    result->generateTiles(rect.fW, rect.fH);
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
    }

    result->updateObjectsMap();

    result->mUnitTypes = settings.fMonsters;

    return result;
}

#include "eSlayerHelpers/eterrstexturesdata.h"

#include "eSlayerHelpers/efileloaderbase.h"

bool eTerrsTexturesData::sLoaded = false;
eStringIdMapVector<eTileTextureData>
eTerrsTexturesData::sTexs;

void eTerrsTexturesData::load() {
    if(sLoaded) return;
    sLoaded = true;

    const auto dir = "Textures";

    const auto path = "terrain/terrain.json";
    std::vector<std::string> names;
    try {
        const auto jdata = eFileLoaderBase::parse(dir, path);
        names = jdata.get<std::vector<std::string>>();
    } catch(...) {
        eRuntimeThrow("Failed to parse " + dir + "/" + path);
    }

    for(const auto& name : names) {
        const auto path = "terrain/" + name + "/" + name + ".json";
        eTileTextureData texs{name};
        if(name == "none") {
            texs.fFlat = true;
            texs.fWallWalkable = false;
            texs.fWallObsticle = false;
            texs.fWallBlocksLight = false;
            texs.fDirs.emplace_back(eWallDirection::none);
        } else {
            try {
                const auto jdata = eFileLoaderBase::parse(dir, path);
                const bool flat = jdata.value("flat", true);
                const bool wallWalkable = jdata.value("wallWalkable", true);
                const bool wallObsticle = jdata.value("wallObsticle", false);
                const bool wallBlocksLight = jdata.value("wallBlocksLight", false);
                texs.fFlat = flat;
                texs.fWallWalkable = wallWalkable;
                texs.fWallObsticle = wallObsticle;
                texs.fWallBlocksLight = wallBlocksLight;

                texs.fDirs.resize(9);
                texs.fDirs[0] = eWallDirection::none;
                texs.fDirs[1] = eWallDirection::topRight;
                texs.fDirs[2] = eWallDirection::rightCorner;
                texs.fDirs[3] = eWallDirection::bottomRight;
                texs.fDirs[4] = eWallDirection::verticalBottom;
                texs.fDirs[5] = eWallDirection::bottomLeft;
                texs.fDirs[6] = eWallDirection::leftCorner;
                texs.fDirs[7] = eWallDirection::topLeft;
                texs.fDirs[8] = eWallDirection::verticalTop;
            } catch(...) {
                eRuntimeThrow("Failed to parse " + dir + "/" + path);
            }
        }
        sTexs.add(name, texs);
    }
}

int eTerrsTexturesData::id(const std::string& name) {
    return sTexs.id(name);
}

eTileTextureData& eTerrsTexturesData::get(const std::string& name) {
    const int id = sTexs.id(name);
    return get(id);
}

eTileTextureData& eTerrsTexturesData::get(const int id) {
    return sTexs.get(id);
}

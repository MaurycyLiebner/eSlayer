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
            texs.fWalkable = false;
            texs.fObsticle = false;
        } else {
            try {
                const auto jdata = eFileLoaderBase::parse(dir, path);
                const bool flat = jdata.value("flat", true);
                const bool walkable = jdata.value("walkable", true);
                const bool obsticle = jdata.value("obsticle", false);
                const bool blocksLight = jdata.value("blocksLight", false);
                texs.fFlat = flat;
                texs.fWalkable = walkable;
                texs.fObsticle = obsticle;
                texs.fBlocksLight = blocksLight;
                if(blocksLight) {
                    texs.fBlockLightDir[0] = eBlockLightDirection::none;
                    texs.fBlockLightDir[1] = eBlockLightDirection::verticalBottom;
                    texs.fBlockLightDir[2] = eBlockLightDirection::rightCorner;
                    texs.fBlockLightDir[3] = eBlockLightDirection::topLeft;
                    texs.fBlockLightDir[4] = eBlockLightDirection::leftCorner;
                    texs.fBlockLightDir[5] = eBlockLightDirection::bottomRight;
                    texs.fBlockLightDir[6] = eBlockLightDirection::verticalTop;
                    texs.fBlockLightDir[7] = eBlockLightDirection::topRight;
                    texs.fBlockLightDir[8] = eBlockLightDirection::bottomLeft;
                }
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

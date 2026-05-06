#include "eSlayerHelpers/eterrstexturesdata.h"

#include "eSlayerHelpers/efileloaderbase.h"
#include "eSlayerHelpers/evectorhelpers.h"

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
        try {
            const auto jdata = eFileLoaderBase::parse(dir, path);
            const int count = jdata.value("count", 0);
            const auto walkable = jdata.value("walkable", std::vector<int>());
            const auto obsticle = jdata.value("obsticle", std::vector<int>());
            texs.fWallsShadow = jdata.value("wallsShadow", true);
            texs.fWallsTransparent = jdata.value("wallsTransparent", true);
            texs.fObsticle.resize(count + 1, false);
            texs.fWalkable.resize(count + 1, false);
            for(int i = 0; i < count; i++) {
                texs.fObsticle[i + 1] = eVectorHelpers::contains(obsticle, i);
                texs.fWalkable[i + 1] = eVectorHelpers::contains(walkable, i);
            }

            if(jdata.contains("borders")) {
                const auto& bordersJS = jdata["borders"];
                const auto handle = [&](const std::string& name) {
                    auto result = bordersJS.value(name, std::vector<int>());
                    for(int& i : result) {
                        i++;
                    }
                    return result;
                };
                texs.fTRBorders = handle("tr");
                texs.fRBorders = handle("r");
                texs.fBRBorders = handle("br");
                texs.fBBorders = handle("b");
                texs.fBLBorders = handle("bl");
                texs.fLBorders = handle("l");
                texs.fTLBorders = handle("tl");
                texs.fTBorders = handle("t");
            }

            const auto floorUseStr = jdata.value("floorUse", "random");
            if(floorUseStr == "tiled") {
                texs.fFloorUse = eFloorUse::tiled;
            } else {
                texs.fFloorUse = eFloorUse::random;
            }
            texs.fFloor = jdata.value("floor", std::vector<int>());

            const auto parse = [&jdata](const std::string& name,
                                        std::vector<int>& tl,
                                        std::vector<int>& tr) {
                if(jdata.contains(name)) {
                    const auto& wallsJS = jdata[name];
                    const auto handle = [&](const std::string& name) {
                        auto result = wallsJS.value(name, std::vector<int>());
                        for(int& i : result) {
                            i++;
                        }
                        return result;
                    };
                    tl = handle("tl");
                    tr = handle("tr");
                }
            };

            parse("walls", texs.fTLWalls, texs.fTRWalls);
            parse("doors", texs.fTLDoors, texs.fTRDoors);
            parse("doorsOpen", texs.fTLDoorsOpen, texs.fTRDoorsOpen);
        } catch(...) {
            eRuntimeThrow("Failed to parse " + dir + "/" + path);
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

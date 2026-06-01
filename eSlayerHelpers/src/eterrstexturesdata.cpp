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

    sTexs.add("none", eTileTextureData());

    for(const auto& name : names) {
        const auto path = "terrain/" + name + "/" + name + ".json";
        eTileTextureData texs{name};
        try {
            const auto jdata = eFileLoaderBase::parse(dir, path);
            const int count = jdata.value("count", 0);
            const auto walkable = jdata.value("walkable", std::vector<int>());
            const auto obstacle = jdata.value("obstacle", std::vector<int>());
            texs.fWallsShadow = jdata.value("wallsShadow", true);
            texs.fWallsTransparent = jdata.value("wallsTransparent", true);
            texs.fWallsThickness = jdata.value("wallsThickness", 0.25f);
            texs.fObstacle.resize(count + 1, false);
            texs.fWalkable.resize(count + 1, false);
            for(int i = 0; i < count; i++) {
                texs.fObstacle[i + 1] = eVectorHelpers::contains(obstacle, i);
                texs.fWalkable[i + 1] = eVectorHelpers::contains(walkable, i);
            }

            const auto floorUseStr = jdata.value("floorUse", "random");
            if(floorUseStr == "tiled") {
                texs.fFloorUse = eFloorUse::tiled;
            } else {
                texs.fFloorUse = eFloorUse::random;
            }
            texs.fFloor = jdata.value("floor", std::vector<int>());

            const auto parse = [&jdata](const std::string& name,
                                        eWallTextures& tl,
                                        eWallTextures& tr,
                                        eWallTextures& br,
                                        eWallTextures& bl) {
                if(!jdata.contains(name)) return;

                const auto& wallsJS = jdata[name];

                const auto handle = [&](const std::string& side) {
                    eWallTextures result;
                    if(!wallsJS.contains(side)) return result;
                    const auto& arr = wallsJS[side];

                    std::vector<std::vector<eWallTexture>> parsed;

                    for(const auto& group : arr) {
                        std::vector<eWallTexture> entries;
                        std::vector<int> ids;

                        for(const auto& item : group) {
                            if(item.is_number_integer()) {
                                const int id = item.get<int>();
                                entries.push_back({id, 0.f, 1.f});
                                ids.push_back(id);
                            } else if(item.is_object()) {
                                const int id = item.value("id", 0);
                                entries.push_back({
                                    id,
                                    item.value("wallMin", 0.f),
                                    item.value("wallMax", 1.f)
                                });

                                ids.push_back(id);
                            }
                        }

                        parsed.push_back(std::move(entries));
                        result.fDataIds.push_back(std::move(ids));
                    }

                    std::vector<size_t> order(parsed.size());
                    std::iota(order.begin(), order.end(), 0);

                    std::sort(order.begin(), order.end(),
                              [&](size_t a, size_t b) {
                                  return parsed[a].size() < parsed[b].size();
                              });

                    decltype(parsed) sortedParsed;
                    decltype(result.fDataIds) sortedIds;

                    for(const size_t i : order) {
                        sortedParsed.push_back(std::move(parsed[i]));
                        sortedIds.push_back(std::move(result.fDataIds[i]));
                    }

                    parsed = std::move(sortedParsed);
                    result.fDataIds = std::move(sortedIds);

                    for(size_t g = 0; g < parsed.size(); ++g) {
                        const auto& group = parsed[g];
                        auto& ids   = result.fDataIds[g];

                        for(size_t i = 0; i < group.size(); ++i) {
                            const auto& e = group[i];

                            result.emplace_back(
                                e.fId + 1,
                                e.fWallMin,
                                e.fWallMax
                                );

                            ids[i] = result.size() - 1;
                        }

                        result.fSizes.emplace_back(group.size());
                    }

                    return result;
                };

                tl = handle("tl");
                tr = handle("tr");
                br = handle("br");
                if(br.empty()) br = tl;
                bl = handle("bl");
                if(bl.empty()) bl = tr;
            };

            parse("walls", texs.fTLWalls, texs.fTRWalls,
                  texs.fBRWalls, texs.fBLWalls);
            parse("doorsOpen", texs.fTLDoorsOpen, texs.fTRDoorsOpen,
                  texs.fBRDoorsOpen, texs.fBLDoorsOpen);
            parse("doors", texs.fTLDoors, texs.fTRDoors,
                  texs.fBRDoors, texs.fBLDoors);
            parse("stairsDown", texs.fTLStairsDown, texs.fTRStairsDown,
                  texs.fBRStairsDown, texs.fBLStairsDown);
            parse("stairsUp", texs.fTLStairsUp, texs.fTRStairsUp,
                  texs.fBRStairsUp, texs.fBLStairsUp);
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

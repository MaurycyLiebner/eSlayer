#include "../include/eSlayerMapGenerator/emapgenerator.h"

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

const eTile& eMap::tile(const int x, const int y) const {
    return mTiles[y][x];
}

std::shared_ptr<eMap>
eMapGenerator::generate(const std::string& name) const {
    const auto result = std::make_shared<eMap>();
    if(name == "town") {
        auto& terrType = result->mTerrainTypes.emplace_back();
        terrType.fName = "town_floor";
        const int w = 20;
        const int h = 40;
        for(int y = 0; y < h; y++) {
            auto& row = result->mTiles.emplace_back();
            row.reserve(w);
            for(int x = 0; x < w; x++) {
                row.emplace_back(eTile{0, 0});
            }
        }
        result->mWidth = w;
        result->mHeight = h;
        result->mTiles[0][0].fTileType = 3;
        result->mTiles[0][1].fTileType = 2;
        result->mTiles[1][0].fTileType = 1;
    }
    return result;
}

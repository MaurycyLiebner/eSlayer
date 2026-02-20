#ifndef EMAPGENERATOR_H
#define EMAPGENERATOR_H

#include "eslayermapgeneratorexport.h"

#include <string>
#include <memory>
#include <vector>

class eMapGenerator;

namespace eSlayerMapGenerator {
    struct ESLAYERMAPGENERATOR_API eTerrainType {
        std::string fName;
    };

    struct ESLAYERMAPGENERATOR_API eTile {
        int fTerrainType;
        int fTileType;
    };

    class ESLAYERMAPGENERATOR_API eMap {
        friend class ::eMapGenerator;
    public:
        int width() const { return mWidth; }
        int height() const { return mHeight; }

        const eTile& tile(const int x, const int y) const;

        const std::vector<eTerrainType>& terrainTypes() const { return mTerrainTypes; }
    private:
        int mWidth = 0;
        int mHeight = 0;
        std::vector<std::vector<eTile>> mTiles;
        std::vector<eTerrainType> mTerrainTypes;
    };

    ESLAYERMAPGENERATOR_API std::shared_ptr<eMap>
    generate(const std::string& name);
}

#endif // EMAPGENERATOR_H

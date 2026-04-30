#ifndef ETERRSTEXTURESDATA_H
#define ETERRSTEXTURESDATA_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

#include <eSlayerHelpers/ewalldirection.h>

struct eTileTextureData {
    std::string fName;
    bool fFlat;
    bool fWallWalkable;
    bool fWallObsticle;
    bool fWallBlocksLight;
    std::vector<eWallDirection> fDirs;
};

class ESLAYERHELPERS_API eTerrsTexturesData {
public:
    eTerrsTexturesData();

    static int id(const std::string& name);
    static eTileTextureData& get(const std::string& name);
    static eTileTextureData& get(const int id);
    static void load();

    static eStringIdMapVector<eTileTextureData> sTexs;
private:
    static bool sLoaded;
};

#endif // ETERRSTEXTURESDATA_H

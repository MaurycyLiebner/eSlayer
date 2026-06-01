#ifndef ETERRSTEXTURESDATA_H
#define ETERRSTEXTURESDATA_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

#include <eSlayerHelpers/ewalldirection.h>

enum class eFloorUse {
    random, tiled
};

struct eWallTexture {
    eWallTexture(const int id,
                 const float wallMin,
                 const float wallMax) :
        fId(id),
        fWallMin(wallMin),
        fWallMax(wallMax) {}

    int fId;
    float fWallMin;
    float fWallMax;
};

struct eWallTextures : public std::vector<eWallTexture> {
    // Sorted by ascending vector size
    std::vector<int> fSizes;
    // Ids in inherited vector
    std::vector<std::vector<int>> fDataIds;
};

struct eTileTextureData {
    std::string fName;

    bool fWallsShadow;
    bool fWallsTransparent;

    std::vector<bool> fObstacle;
    std::vector<bool> fWalkable;

    eFloorUse fFloorUse;
    std::vector<int> fFloor;

    float fWallsThickness;

    eWallTextures fTLWalls;
    eWallTextures fTRWalls;

    eWallTextures fBLWalls;
    eWallTextures fBRWalls;

    eWallTextures fTLDoors;
    eWallTextures fTRDoors;

    eWallTextures fBLDoors;
    eWallTextures fBRDoors;

    eWallTextures fTLDoorsOpen;
    eWallTextures fTRDoorsOpen;

    eWallTextures fBLDoorsOpen;
    eWallTextures fBRDoorsOpen;

    eWallTextures fTLStairsDown;
    eWallTextures fTRStairsDown;

    eWallTextures fBLStairsDown;
    eWallTextures fBRStairsDown;

    eWallTextures fTLStairsUp;
    eWallTextures fTRStairsUp;

    eWallTextures fBLStairsUp;
    eWallTextures fBRStairsUp;
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

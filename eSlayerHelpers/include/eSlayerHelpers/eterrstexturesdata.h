#ifndef ETERRSTEXTURESDATA_H
#define ETERRSTEXTURESDATA_H

#include "eslayerhelpersexport.h"

#include "estringidmapvector.h"

#include <eSlayerHelpers/ewalldirection.h>

enum class eFloorUse {
    random, tiled
};

struct eTileTextureData {
    std::string fName;

    std::vector<bool> fObsticle;
    std::vector<bool> fWalkable;

    eFloorUse fFloorUse;
    std::vector<int> fFloor;

    std::vector<int> fTRBorders;
    std::vector<int> fRBorders;
    std::vector<int> fBRBorders;
    std::vector<int> fBBorders;
    std::vector<int> fBLBorders;
    std::vector<int> fLBorders;
    std::vector<int> fTLBorders;
    std::vector<int> fTBorders;

    std::vector<int> fTLWalls;
    std::vector<int> fTRWalls;

    std::vector<int> fTLDoors;
    std::vector<int> fTRDoors;

    std::vector<int> fTLDoorsOpen;
    std::vector<int> fTRDoorsOpen;
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

#include "eSlayerHelpers/etile.h"

#include "eSlayerHelpers/eterrstexturesdata.h"

bool eTile::walkable() const {
    if(fTerrainType == 0) return false;
    if(fTileType == 0) return false;
    const auto& info = eTerrsTexturesData::get(fTerrainType);
    return info.fWalkable[fTileType];
}

bool eTile::obstacle() const {
    if(fTerrainType == 0) return false;
    if(fTileType == 0) return false;
    const auto& info = eTerrsTexturesData::get(fTerrainType);
    return info.fObstacle[fTileType];
}

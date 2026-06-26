#include "eSlayerHelpers/etile.h"

#include "eSlayerHelpers/eterrstexturesdata.h"
#include "eSlayerHelpers/epacket.h"

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

void eTile::read(ePacket& p) {
    p >> fTerrainType;
    p >> fTileType;

    bool hasWall;
    p >> hasWall;

    if(hasWall) {
        p >> fWallTL;
        p >> fWallTR;

        p >> fStairsTL;
        p >> fStairsTR;
    }

    p.read8(fOverlays);
}

void eTile::write(ePacket& p) const {
    p << fTerrainType;
    p << fTileType;

    const bool hasWall = fWallTL || fWallTR;
    p << hasWall;

    if(hasWall) {
        p << fWallTL;
        p << fWallTR;

        p << fStairsTL;
        p << fStairsTR;
    }

    p.write8(fOverlays);
}

#ifndef ETILE_H
#define ETILE_H

#include "eslayerhelpersexport.h"

#include <cstdint>

struct ESLAYERHELPERS_API eTile {
    uint8_t fTerrainType;
    uint8_t fTileType;

    // 0 - no wall
    uint8_t fWallTL;
    uint8_t fWallTR;

    static uint8_t encodeWall(
        const bool wall,
        const bool doors,
        const uint8_t type) {
        return (static_cast<uint8_t>(wall) << 0) |
               (static_cast<uint8_t>(doors) << 1) |
               (type << 2);
    }

    static void decodeWall(
        const uint8_t encoded,
        bool& wall,
        bool& doors,
        uint8_t& type) {
        wall  = (encoded >> 0) & 1;
        doors = (encoded >> 1) & 1;
        type  = (encoded >> 2);
    }
};

#endif // ETILE_H

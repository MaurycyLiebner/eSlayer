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
        const bool open,
        const uint8_t type) {
        return (static_cast<uint8_t>(wall) << 0) |
               (static_cast<uint8_t>(doors) << 1) |
               (static_cast<uint8_t>(open) << 2) |
               (type << 3);
    }

    static bool doors(const uint8_t encoded) {
        return (encoded >> 1) & 1;
    }

    static bool open(const uint8_t encoded) {
        return (encoded >> 2) & 1;
    }

    static void setOpen(uint8_t& encoded, const bool open) {
        if(open) {
            encoded |= (1u << 2);
        } else {
            encoded &= ~(1u << 2);
        }
    }

    static uint8_t type(const uint8_t encoded) {
        return (encoded >> 3);
    }

    static void decodeWall(
        const uint8_t encoded,
        bool& wall,
        bool& doors,
        bool& open,
        uint8_t& type) {
        wall  = (encoded >> 0) & 1;
        doors = (encoded >> 1) & 1;
        open  = (encoded >> 2) & 1;
        type  = (encoded >> 3);
    }
};

#endif // ETILE_H

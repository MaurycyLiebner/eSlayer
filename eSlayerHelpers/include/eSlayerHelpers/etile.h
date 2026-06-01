#ifndef ETILE_H
#define ETILE_H

#include "eslayerhelpersexport.h"

#include <cstdint>

struct ESLAYERHELPERS_API eTile {
    uint8_t fTerrainType;
    uint8_t fTileType;

    uint8_t fWallTL;
    uint8_t fWallTR;

    uint8_t fStairsTL;
    uint8_t fStairsTR;

    bool walkable() const;
    bool obstacle() const;

    static uint8_t encodeWall(
        const bool wall,
        const bool doors,
        const bool open,
        const bool other,
        const uint8_t type) {
        return (static_cast<uint8_t>(wall) << 0) |
               (static_cast<uint8_t>(doors) << 1) |
               (static_cast<uint8_t>(open) << 2) |
               (static_cast<uint8_t>(other) << 3) |
               (type << 4);
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

    static bool other(const uint8_t encoded) {
        return (encoded >> 3) & 1;
    }

    static uint8_t type(const uint8_t encoded) {
        return (encoded >> 4);
    }

    static bool stairs(const uint8_t encoded) {
        return (encoded >> 0) & 1;
    }

    static void setStairs(uint8_t& encoded, const bool stairs) {
        if(stairs) {
            encoded |= (1u << 0);
        } else {
            encoded &= ~(1u << 0);
        }
    }

    static bool stairsUp(const uint8_t encoded) {
        return (encoded >> 1) & 1;
    }

    static void setStairsUp(uint8_t& encoded, const bool up) {
        if(up) {
            encoded |= (1u << 1);
        } else {
            encoded &= ~(1u << 1);
        }
    }

    static uint8_t stairsType(const uint8_t encoded) {
        return (encoded >> 2);
    }

    static uint8_t encodeStairs(
        const bool stairs,
        const bool stairsUp,
        const uint8_t type) {
        return (static_cast<uint8_t>(stairs) << 0) |
               (static_cast<uint8_t>(stairsUp) << 1) |
               (static_cast<uint8_t>(type) << 2);
    }
};

#endif // ETILE_H

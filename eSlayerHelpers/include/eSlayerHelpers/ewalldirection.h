#ifndef EWALLDIRECTION_H
#define EWALLDIRECTION_H

#include <cstdint>

enum class eWallDirection {
    none,
    topRight,
    rightCorner,
    bottomRight,
    verticalBottom,
    bottomLeft,
    leftCorner,
    topLeft,
    verticalTop
};

enum class eWallType : uint8_t {
    topLeft, topRight
};

#endif // EWALLDIRECTION_H

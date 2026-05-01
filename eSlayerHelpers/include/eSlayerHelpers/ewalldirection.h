#ifndef EWALLDIRECTION_H
#define EWALLDIRECTION_H

enum class eWallDirection {
    none,
    topRight, rightCorner,
    bottomRight, verticalBottom,
    bottomLeft, leftCorner,
    topLeft, verticalTop
};

enum class eWallType {
    topLeft, topRight
};

#endif // EWALLDIRECTION_H

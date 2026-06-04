#ifndef EDUNGEONGENERATOR_H
#define EDUNGEONGENERATOR_H

#include "../include/eSlayerMapGenerator/echamber.h"

enum class eDir {
    none,
    topLeft, topRight,
    bottomRight, bottomLeft
};

namespace eDirHelpers {
    eDir flip(const eDir dir);
}

class eDungeonGenerator {
public:
    static void generate(
        const eRect& rect,
        std::vector<eChamber>& chambers,
        std::vector<eRect>& doors);
};

#endif // EDUNGEONGENERATOR_H

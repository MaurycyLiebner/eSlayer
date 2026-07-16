#ifndef EDUNGEONGENERATOR_H
#define EDUNGEONGENERATOR_H

#include "eSlayerMapGenerator/echamber.h"

struct eAreaSettings;

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
        std::vector<eRect>& doors,
        const eAreaSettings& settings);
};

#endif // EDUNGEONGENERATOR_H

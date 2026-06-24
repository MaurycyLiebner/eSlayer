#ifndef EBLUEPRINTS_H
#define EBLUEPRINTS_H

#include "estringidmapvector.h"

#include <cstdint>
#include <vector>

struct eBlueprintObject {
    uint8_t fObjId;
    uint8_t fX;
    uint8_t fY;
};

struct eBlueprint {
    uint8_t fWidth;
    uint8_t fHeight;

    std::vector<eBlueprintObject> fObjects;
};

class eBlueprints {
public:
    static eStringIdMapVector<eBlueprint> sBlueprints;

    static void load();
private:
    static bool sLoaded;
};

#endif // EBLUEPRINTS_H
